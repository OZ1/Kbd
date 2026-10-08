namespace KbdImage.Rendering;

using Model;

using static Math;

/// <summary>Рисуетъ клавіатуру въ логическихъ единицахъ (LogicalSize) на любую IPainterSurface.</summary>
public sealed class KeyboardPainter
{
	public bool HasLayout => Keys.Count > 0;
	const float Gap = 3f;
	const float PenW = 3f;

	static readonly Color KeyFill      = Color.White;
	static readonly Color SysFill      = Color.FromArgb(0xE6, 0xE6, 0xE6);
	static readonly Color SysText      = Color.FromArgb(0x78, 0x78, 0x78);
	static readonly Color IconColor    = Color.FromArgb(0x50, 0x50, 0x50);
	static readonly Color DeadFill     = Color.FromArgb(0xDC, 0xDC, 0xF6);
	static readonly Color Outline      = Color.Black;
	static readonly Color BrickOutline = Color.FromArgb(0x4A, 0x10, 0x0E);

	// По слотамъ: обычная, Shift, Ctrl, Kana, Kana+Shift, Caps, Caps+Shift
	static readonly Color[] SlotColor =
	[
		Color.FromArgb(0xFF, 0x00, 0x00),
		Color.FromArgb(0x80, 0x00, 0x00),
		Color.FromArgb(0x00, 0x80, 0x80),
		Color.FromArgb(0x80, 0x00, 0x80),
		Color.FromArgb(0x4B, 0x00, 0x82),
		Color.FromArgb(0x00, 0x90, 0x00),
		Color.FromArgb(0x00, 0x50, 0x00),
	];
	static readonly float[] SlotSize = [20, 19, 9, 19, 18, 14, 14];
	static readonly (int Col, int Row)[] SlotPos = [(0, 0), (0, 1), (0, 2), (1, 0), (1, 1), (2, 0), (2, 1)];

	// Центры столбцовъ и строкъ ячеекъ внутри кнопки (строка 0 — нижняя)
	static readonly float[] ColX = [14, 39, 61];
	static readonly float[] RowY = [63, 39, 18];

	public static SizeF LogicalSize { get; } = new(Geometry.Width, Geometry.Height);

	static readonly Dictionary<int, string> SysLabels = new()
	{
		{ 0x67, "Home" }, { 0x68, "↑" }, { 0x69, "PgUp" }, { 0x64, "←"   }, { 0x66, "→"   },
		{ 0x61, "End"  }, { 0x62, "↓" }, { 0x63, "PgDn" }, { 0x60, "Ins" }, { 0x6E, "Del" },
	};

	readonly List<(KeyDef Key, List<Cell> Cells, bool Brick, string? SysLabel)> Keys = [];

	/// <summary>Загружаетъ разкладку; безъ нея рисуются только кнопки.</summary>
	public void SetLayout(KBDTABLES layout)
	{
		Keys.Clear();
		foreach (KeyDef k in Geometry.Keys)
		{
			int vk = k.Kind is KeyKind.Data or KeyKind.Enter ? layout.Vk(k.Scan, k.Ext) : -1;
			if (vk < 0)
			{
				Keys.Add((k, [], false, null));
				continue;
			}
			List<Cell> cells = Cells.Resolve(layout, vk);
			// Сѵстемная подпись цифровой кнопки (Home, ↑ …), если Shift-ячейка пуста
			string? label = cells.Exists(c => c.Slot == Slot.Shift) ? null : SysLabels.GetValueOrDefault(vk);
			Keys.Add((k, cells, Cells.DiffersFromSystem(layout, k.Scan, k.Ext, vk), label));
		}
	}

	/// <summary>Кнопки безъ буквъ: рамки, сѵстемныя подписи и значки. Не зависитъ отъ разкладки — рисуется и безъ файла.</summary>
	public static void DrawBackground(IPainterSurface s)
	{
		foreach (KeyDef key in Geometry.Keys)
			DrawKey(s, key);
	}

	/// <summary>Буквы поверхъ кнопокъ (и кирпичная обводка отличающихся кнопокъ).</summary>
	public void DrawLetters(IPainterSurface s)
	{
		foreach ((KeyDef key, List<Cell> cells, bool brick, string? label) in Keys)
			DrawLetters(s, key, cells, brick, label);
	}

	public static void PaintBackground(Graphics g, RectangleF dest)
	{
		using GdiSurface? s = GdiSurface.Fit(g, dest);
		if (s != null)
			DrawBackground(s);
	}

	public void PaintLetters(Graphics g, RectangleF dest)
	{
		using GdiSurface? s = GdiSurface.Fit(g, dest);
		if (s != null)
			DrawLetters(s);
	}

	/// <summary>Кнопки и буквы вмѣстѣ — для PNG и печати.</summary>
	public void PaintAll(Graphics g, RectangleF dest)
	{
		PaintBackground(g, dest);
		PaintLetters(g, dest);
	}

	/// <summary>SVG-документъ; measure — ширина строки въ пикселяхъ при данномъ размѣрѣ шрифта.</summary>
	public string ToSvg(int width, int height, Func<string, float, float> measure)
	{
		SvgSurface s = new(measure);
		DrawBackground(s);
		DrawLetters(s);
		return s.ToSvg(width, height);
	}

	static RectangleF Inner(KeyDef key)
		=> new(key.X + Gap + PenW / 2, key.Y + Gap + PenW / 2, key.W - 2 * Gap - PenW, key.H - 2 * Gap - PenW);

	static void DrawKey(IPainterSurface s, KeyDef key)
	{
		RectangleF r = Inner(key);
		s.FillRect(r, key.Kind is KeyKind.Label or KeyKind.Shift or KeyKind.Win or KeyKind.Menu ? SysFill : KeyFill);
		s.StrokeRect(r, Outline, PenW);

		float left = key.X + Gap;
		float mid = key.Y + RowY[1];
		switch (key.Kind)
		{
			case KeyKind.Label:
				bool twoLines = key.Label.Contains('\n');
				foreach ((string line, int i) in key.Label.Split('\n').Select((l, i) => (l, i)))
					DrawNear(s, line, twoLines ? 26 : 25, SysText, left + 8 + (twoLines ? 0 : 4), twoLines ? mid + (i - 0.5f) * 34 : key.Y + 30);
				break;
			case KeyKind.Menu     : DrawMenu (s, left + 14, key.Y + 21); break;
			case KeyKind.Win      : DrawWin  (s, left + 13, key.Y + 21); break;
			case KeyKind.Tab      : DrawNear(s, "↹" , 38, Color.Black, left + 13, mid + 4); break; // DrawTab(s, left + 10 + 20, mid)
			case KeyKind.Shift    : DrawNear (s, "⇧", 34, Color.Black, left + 10, mid + 4); break;
			case KeyKind.BackSpace: DrawRight(s, "⌫", 34, Color.Black, key, mid + 4); break;
			case KeyKind.Enter    : DrawRight(s, "Ввод", 25, SysText, key, key.Y + 30);
				/**/           DrawEnterArrow(s, key, s.Measure("Ввод", 25), key.Y + RowY[0] - 6);
				break;
		}
	}

	static void DrawLetters(IPainterSurface s, KeyDef key, List<Cell> cells, bool brick, string? label)
	{
		if (brick)
			s.StrokeRect(Inner(key), BrickOutline, PenW);
		float left = key.X + Gap;
		foreach (Cell c in cells)
		{
			(int col, int row) = SlotPos[(int)c.Slot];
			float x = left + ColX[col];
			float y = key.Y + RowY[row];
			if (c.Boxed)
				DrawBoxed(s, c, x, y);
			else
				DrawGlyph(s, c, x, y);
		}
		if (label != null)
			DrawNear(s, label, label.Length == 1 ? 22 : 10, SysText, left + 4, key.Y + RowY[1]);
	}

	static void DrawNear(IPainterSurface s, string text, float px, Color color, float x, float y)
		=> s.Text(text, px, color, new(x, y - 30, 200, 60), false);

	static void DrawRight(IPainterSurface s, string text, float px, Color color, KeyDef key, float y)
		=> DrawNear(s, text, px, color, key.X + key.W - Gap - PenW - 10 - s.Measure(text, px), y);

	/// <summary>Значокъ Tab: двѣ длинныя стрѣлки къ коротенькимъ вертикальнымъ палочкамъ.</summary>
	static void DrawTab(IPainterSurface s, float cx, float cy)
	{
		const float half = 17, head = 5, bar = 7, lw = 2.5f;
		void Arrow(float y, int dir)
		{
			float tip = cx + dir * half;
			float bx = cx - dir * half;
			s.Line(new(bx, y), new(tip - dir * 1, y), Color.Black, lw);
			s.Line(new(tip, y), new(tip - dir * head, y - head), Color.Black, lw);
			s.Line(new(tip, y), new(tip - dir * head, y + head), Color.Black, lw);
			s.Line(new(tip + dir * 3, y - bar), new(tip + dir * 3, y + bar), Color.Black, lw);
		}
		Arrow(cy - 9, 1);
		Arrow(cy + 9, -1);
	}

	/// <summary>Стрѣлка «↵» подъ надписью: горизонталь въ половину ширины слова «Ввод», справа вверхъ.</summary>
	static void DrawEnterArrow(IPainterSurface s, KeyDef key, float width, float y)
	{
		const float head = 4, lw = 2f, rise = 9;
		float right = key.X + key.W - Gap - PenW - 10 - width * 0.1f;
		float x0 = right - width * 0.8f;
		s.Line(new(x0, y), new(right, y), Color.Black, lw);
		s.Line(new(right, y), new(right, y - rise), Color.Black, lw);
		s.Line(new(x0, y), new(x0 + head, y - head), Color.Black, lw);
		s.Line(new(x0, y), new(x0 + head, y + head), Color.Black, lw);
	}

	static void DrawGlyph(IPainterSurface s, Cell c, float x, float y)
	{
		float px = SlotSize[(int)c.Slot];
		if (c.Dead)
		{
			float w = Max(s.Measure(c.Text, px), 12) + 8;
			s.FillRect(new(x - w / 2, y - px * 0.75f, w, px * 1.5f), DeadFill);
		}
		s.Text(c.Text, px, SlotColor[(int)c.Slot], new(x - 60, y - 30, 120, 60), true);
	}

	static void DrawBoxed(IPainterSurface s, Cell c, float x, float y)
	{
		const float lh = 10;
		string[] lines = c.Text.Split('\n');
		float w = lines.Max(line => s.Measure(line, 9)) + 6;
		float h = lines.Length * lh + 4;
		float top = y - h / 2;
		s.StrokeRect(new(x - w / 2, top, w, h), SlotColor[(int)c.Slot], 1.2f, true);
		for (int i = 0; i < lines.Length; i++)
			s.Text(lines[i], 9, SlotColor[(int)c.Slot], new(x - 40, top - 3 + i * lh, 80, 20), true);
	}

	static void DrawWin(IPainterSurface s, float x, float y)
	{
		PointF P(float px, float py) => new(x + px * 0.83f, y + py * 0.72f);
		s.FillPolygon([P( 0,  4), P(13,  2), P(13, 14), P( 0, 14)], IconColor);
		s.FillPolygon([P(15,  2), P(29,  0), P(29, 14), P(15, 14)], IconColor);
		s.FillPolygon([P( 0, 16), P(13, 16), P(13, 28), P( 0, 26)], IconColor);
		s.FillPolygon([P(15, 16), P(29, 16), P(29, 30), P(15, 28)], IconColor);
	}

	static void DrawMenu(IPainterSurface s, float x, float y)
	{
		s.StrokeRect(new(x, y, 13, 17), IconColor, 1.5f);
		for (int i = 0; i < 3; i++)
			s.Line(new(x + 3, y + 5 + i * 3.6f), new(x + 10, y + 5 + i * 3.6f), IconColor, 1.5f);
	}
}
