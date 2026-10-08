using System.Drawing.Drawing2D;
using System.Drawing.Text;

namespace KbdImage.Rendering;

/// <summary>Рисуетъ на Graphics. Кисти, перья и шрифты кэшируются на всё время работы: перерисовка идётъ при каждомъ измѣненіи размѣра.</summary>
public sealed class GdiSurface : IPainterSurface, IDisposable
{
	static readonly Dictionary<int, SolidBrush> Brushes = [];
	static readonly Dictionary<(int, float, bool), Pen> Pens = [];
	static readonly Dictionary<float, Font> Fonts = [];

	static readonly StringFormat Centered = new(StringFormat.GenericTypographic)
	{
		Alignment = StringAlignment.Center,
		LineAlignment = StringAlignment.Center,
		FormatFlags = StringFormatFlags.NoWrap,
	};

	static readonly StringFormat Near = new(StringFormat.GenericTypographic)
	{
		LineAlignment = StringAlignment.Center,
		FormatFlags = StringFormatFlags.NoWrap,
	};

	readonly Graphics _g;
	readonly GraphicsState? _state;

	public GdiSurface(Graphics g) => _g = g;

	GdiSurface(Graphics g, GraphicsState state)
	{
		_g = g;
		_state = state;
	}

	/// <summary>Вписываетъ клавіатуру въ dest, сохраняя пропорціи и центрируя; null, если рисовать негдѣ.</summary>
	public static GdiSurface? Fit(Graphics g, RectangleF dest)
	{
		float s = Math.Min(dest.Width / KeyboardPainter.LogicalSize.Width, dest.Height / KeyboardPainter.LogicalSize.Height);
		if (s <= 0)
			return null;
		GraphicsState state = g.Save();
		g.SmoothingMode = SmoothingMode.AntiAlias;
		g.TextRenderingHint = TextRenderingHint.AntiAlias;
		g.PixelOffsetMode = PixelOffsetMode.HighQuality;
		g.TranslateTransform(dest.X + (dest.Width - KeyboardPainter.LogicalSize.Width * s) / 2, dest.Y + (dest.Height - KeyboardPainter.LogicalSize.Height * s) / 2);
		g.ScaleTransform(s, s);
		return new GdiSurface(g, state);
	}

	public void Dispose()
	{
		if (_state != null)
			_g.Restore(_state);
	}

	static SolidBrush Brush(Color c)
	{
		if (!Brushes.TryGetValue(c.ToArgb(), out SolidBrush? b))
			Brushes[c.ToArgb()] = b = new SolidBrush(c);
		return b;
	}

	static Pen Pen(Color c, float width, bool dashed)
	{
		if (!Pens.TryGetValue((c.ToArgb(), width, dashed), out Pen? p))
			Pens[(c.ToArgb(), width, dashed)] = p = new Pen(c, width) { DashStyle = dashed ? DashStyle.Dash : DashStyle.Solid };
		return p;
	}

	static Font Font(float px)
	{
		if (!Fonts.TryGetValue(px, out Font? f))
			Fonts[px] = f = new Font("Segoe UI", px, FontStyle.Regular, GraphicsUnit.Pixel);
		return f;
	}

	public void FillRect(RectangleF r, Color color) => _g.FillRectangle(Brush(color), r);

	public void StrokeRect(RectangleF r, Color color, float width, bool dashed = false)
		=> _g.DrawRectangle(Pen(color, width, dashed), r.X, r.Y, r.Width, r.Height);

	public void FillPolygon(PointF[] points, Color color) => _g.FillPolygon(Brush(color), points);

	public void Line(PointF a, PointF b, Color color, float width) => _g.DrawLine(Pen(color, width, false), a, b);

	public void Text(string text, float px, Color color, RectangleF box, bool center)
		=> _g.DrawString(text, Font(px), Brush(color), box, center ? Centered : Near);

	public float Measure(string text, float px) => _g.MeasureString(text, Font(px), 1000, Centered).Width;
}
