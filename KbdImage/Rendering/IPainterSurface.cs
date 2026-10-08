namespace KbdImage.Rendering;

/// <summary>Минимумъ примитивовъ, черезъ которые клавіатура рисуется и на растръ (GDI+), и въ векторъ (SVG).</summary>
public interface IPainterSurface
{
	void FillRect(RectangleF r, Color color);

	void StrokeRect(RectangleF r, Color color, float width, bool dashed = false);

	void FillPolygon(PointF[] points, Color color);

	void Line(PointF a, PointF b, Color color, float width);

	/// <summary>Одна строка текста Segoe UI размеромъ px; по вертикали — по серединѣ box, по горизонтали — по серединѣ или по лѣвому краю.</summary>
	void Text(string text, float px, Color color, RectangleF box, bool center);

	float Measure(string text, float px);
}
