using System.ComponentModel;

using KbdImage.Rendering;

namespace KbdImage;

/// <summary>Элементъ управленія, показывающій Canvas. Отдѣльный открытый классъ, чтобы дизайнеръ формъ могъ его создать.</summary>
sealed class Canvas : Control
{
	public Canvas()
	{
		DoubleBuffered = true;
		ResizeRedraw = true;
	}

	[Browsable(false)]
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
	public KeyboardPainter Painter { get; } = new();

	/// <summary>Поле вокругъ клавіатуры.</summary>
	[DefaultValue(8)]
	public int Pad
	{
		get; set
		{
			field = value;
			Invalidate();
		}
	} = 8;

	RectangleF Area => new(Pad, Pad, Width - 2 * Pad, Height - 2 * Pad);

	/// <summary>Фонъ съ кнопками рисуется всегда, даже когда файлъ не выбранъ. Цвѣтъ фона — отъ формы, т. е. отъ темы ОС; кнопки всегда бѣлыя, какъ на картинкѣ.</summary>
	protected override void OnPaintBackground(PaintEventArgs e)
	{
		base.OnPaintBackground(e);
		KeyboardPainter.PaintBackground(e.Graphics, Area);
	}

	/// <summary>Только буквы; безъ файла ихъ нѣтъ.</summary>
	protected override void OnPaint(PaintEventArgs e)
	{
		Painter.PaintLetters(e.Graphics, Area);
	}
}
