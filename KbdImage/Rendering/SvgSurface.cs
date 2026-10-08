using System.Globalization;
using System.Security;
using System.Text;

namespace KbdImage.Rendering;

/// <summary>Собираетъ SVG. Ширину текста (для рамокъ) спрашиваетъ у measure — въ Windows это GDI+.</summary>
public sealed class SvgSurface(Func<string, float, float> measure) : IPainterSurface
{
	readonly StringBuilder _body = new();

	static string N(float v) => v.ToString("0.##", CultureInfo.InvariantCulture);

	static string Rgb(Color c) => $"#{c.R:x2}{c.G:x2}{c.B:x2}";

	public void FillRect(RectangleF r, Color color)
		=> _body.Append($"<rect x=\"{N(r.X)}\" y=\"{N(r.Y)}\" width=\"{N(r.Width)}\" height=\"{N(r.Height)}\" fill=\"{Rgb(color)}\"/>\n");

	public void StrokeRect(RectangleF r, Color color, float width, bool dashed = false)
		=> _body.Append($"<rect x=\"{N(r.X)}\" y=\"{N(r.Y)}\" width=\"{N(r.Width)}\" height=\"{N(r.Height)}\" fill=\"none\" stroke=\"{Rgb(color)}\" stroke-width=\"{N(width)}\"{(dashed ? $" stroke-dasharray=\"{N(3 * width)} {N(width)}\"" : "")}/>\n");

	public void FillPolygon(PointF[] points, Color color)
		=> _body.Append($"<polygon points=\"{string.Join(' ', points.Select(p => $"{N(p.X)},{N(p.Y)}"))}\" fill=\"{Rgb(color)}\"/>\n");

	public void Line(PointF a, PointF b, Color color, float width)
		=> _body.Append($"<line x1=\"{N(a.X)}\" y1=\"{N(a.Y)}\" x2=\"{N(b.X)}\" y2=\"{N(b.Y)}\" stroke=\"{Rgb(color)}\" stroke-width=\"{N(width)}\"/>\n");

	public void Text(string text, float px, Color color, RectangleF box, bool center)
		=> _body.Append($"<text x=\"{N(center ? box.X + box.Width / 2 : box.X)}\" y=\"{N(box.Y + box.Height / 2)}\" font-size=\"{N(px)}\" fill=\"{Rgb(color)}\"{(center ? " text-anchor=\"middle\"" : "")}>{SecurityElement.Escape(text)}</text>\n");

	public float Measure(string text, float px) => measure(text, px);

	/// <summary>Весь документъ; viewBox — логическій размѣръ, width/height — размѣръ картинки.</summary>
	public string ToSvg(int width, int height)
		=> $"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"{width}\" height=\"{height}\" viewBox=\"0 0 {N(KeyboardPainter.LogicalSize.Width)} {N(KeyboardPainter.LogicalSize.Height)}\" font-family=\"Segoe UI, sans-serif\" dominant-baseline=\"central\">\n{_body}</svg>\n";
}
