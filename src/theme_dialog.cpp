/***********************************************************************
 *
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "theme_dialog.h"

#include "color_button.h"
#include "font_combobox.h"
#include "image_button.h"
#include "theme.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QFile>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

ThemeDialog::ThemeDialog(Theme& theme, QWidget* parent)
	: QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	m_theme(theme)
{
	setWindowTitle(tr("Modify Theme"));
	setWindowModality(Qt::WindowModal);

	// Create name edit
	m_name = new QLineEdit(this);
	m_name->setText(m_theme.name());
	connect(m_name, SIGNAL(textChanged(QString)), this, SLOT(checkNameAvailable()));

	QHBoxLayout* name_layout = new QHBoxLayout;
	name_layout->setMargin(0);
	name_layout->addWidget(new QLabel(tr("Name:"), this));
	name_layout->addWidget(m_name);


	QTabWidget* tabs = new QTabWidget(this);

	// Create background group
	QWidget* tab = new QWidget(this);
	tabs->addTab(tab, tr("Background"));

	m_background_type = new QComboBox(tab);
	m_background_type->addItems(QStringList() << tr("No Image") << tr("Tiled") << tr("Centered") << tr("Stretched") << tr("Scaled") << tr("Zoomed"));
	m_background_type->setCurrentIndex(m_theme.backgroundType());
	connect(m_background_type, SIGNAL(activated(int)), this, SLOT(renderPreview()));

	m_background_color = new ColorButton(tab);
	m_background_color->setColor(m_theme.backgroundColor());
	connect(m_background_color, SIGNAL(changed(QColor)), this, SLOT(renderPreview()));

	m_background_image = new ImageButton(tab);
	m_background_image->setImage(m_theme.backgroundImage(), m_theme.backgroundPath());
	connect(m_background_image, SIGNAL(changed(QString)), this, SLOT(imageChanged()));

	m_clear_image = new QPushButton(tr("Remove"), this);
	connect(m_clear_image, SIGNAL(clicked()), m_background_image, SLOT(unsetImage()));

	QVBoxLayout* image_layout = new QVBoxLayout;
	image_layout->setSpacing(0);
	image_layout->addWidget(m_background_image);
	image_layout->addWidget(m_clear_image);

	QFormLayout* background_layout = new QFormLayout(tab);
	background_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	background_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	background_layout->setLabelAlignment(Qt::AlignRight);
	background_layout->addRow(tr("Type:"), m_background_type);
	background_layout->addRow(tr("Color:"), m_background_color);
	background_layout->addRow(tr("Image:"), image_layout);


	// Create foreground group
	tab = new QWidget(this);
	tabs->addTab(tab, tr("Foreground"));

	m_foreground_color = new ColorButton(tab);
	m_foreground_color->setColor(m_theme.foregroundColor());
	connect(m_foreground_color, SIGNAL(changed(QColor)), this, SLOT(renderPreview()));

	m_foreground_opacity = new QSpinBox(tab);
	m_foreground_opacity->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_opacity->setSuffix("%");
	m_foreground_opacity->setRange(theme.foregroundOpacity().minimumValue(), theme.foregroundOpacity().maximumValue());
	m_foreground_opacity->setValue(m_theme.foregroundOpacity());
	m_foreground_opacity->setToolTip(tr("Opacity"));
	connect(m_foreground_opacity, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_foreground_width = new QSpinBox(tab);
	m_foreground_width->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_width->setSuffix(tr(" pixels"));
	m_foreground_width->setRange(theme.foregroundWidth().minimumValue(), theme.foregroundWidth().maximumValue());
	m_foreground_width->setValue(m_theme.foregroundWidth());
	connect(m_foreground_width, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_foreground_position = new QComboBox(tab);
	m_foreground_position->addItems(QStringList() << tr("Left") << tr("Centered") << tr("Right") << tr("Stretched"));
	m_foreground_position->setCurrentIndex(m_theme.foregroundPosition());
	m_foreground_position->setToolTip(tr("Position"));

	m_foreground_rounding = new QSpinBox(tab);
	m_foreground_rounding->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_rounding->setSuffix(tr(" pixels"));
	m_foreground_rounding->setRange(theme.foregroundRounding().minimumValue(), theme.foregroundRounding().maximumValue());
	m_foreground_rounding->setValue(m_theme.foregroundRounding());
	connect(m_foreground_rounding, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_foreground_margin = new QSpinBox(tab);
	m_foreground_margin->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_margin->setSuffix(tr(" pixels"));
	m_foreground_margin->setRange(theme.foregroundMargin().minimumValue(), theme.foregroundMargin().maximumValue());
	m_foreground_margin->setValue(m_theme.foregroundMargin());
	connect(m_foreground_margin, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_foreground_padding = new QSpinBox(tab);
	m_foreground_padding->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_foreground_padding->setSuffix(tr(" pixels"));
	m_foreground_padding->setRange(theme.foregroundPadding().minimumValue(), theme.foregroundPadding().maximumValue());
	m_foreground_padding->setValue(m_theme.foregroundPadding());
	connect(m_foreground_padding, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_blur = new QGroupBox(tr("Blur Background"), tab);
	m_blur->setCheckable(true);
	m_blur->setChecked(m_theme.blurEnabled());
	connect(m_blur, SIGNAL(clicked()), this, SLOT(renderPreview()));

	m_blur_radius = new QSpinBox(m_blur);
	m_blur_radius->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_blur_radius->setSuffix(tr(" pixels"));
	m_blur_radius->setRange(theme.blurRadius().minimumValue(), theme.blurRadius().maximumValue());
	m_blur_radius->setValue(m_theme.blurRadius());
	connect(m_blur_radius, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_shadow = new QGroupBox(tr("Drop Shadow"), tab);
	m_shadow->setCheckable(true);
	m_shadow->setChecked(m_theme.shadowEnabled());
	connect(m_shadow, SIGNAL(clicked()), this, SLOT(renderPreview()));

	m_shadow_color = new ColorButton(m_shadow);
	m_shadow_color->setColor(m_theme.shadowColor());
	connect(m_shadow_color, SIGNAL(changed(QColor)), this, SLOT(renderPreview()));

	m_shadow_radius = new QSpinBox(m_shadow);
	m_shadow_radius->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_shadow_radius->setSuffix(tr(" pixels"));
	m_shadow_radius->setRange(theme.shadowRadius().minimumValue(), theme.shadowRadius().maximumValue());
	m_shadow_radius->setValue(m_theme.shadowRadius());
	connect(m_shadow_radius, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_shadow_offset = new QSpinBox(m_shadow);
	m_shadow_offset->setCorrectionMode(QSpinBox::CorrectToNearestValue);
	m_shadow_offset->setSuffix(tr(" pixels"));
	m_shadow_offset->setRange(theme.shadowOffset().minimumValue(), theme.shadowOffset().maximumValue());
	m_shadow_offset->setValue(m_theme.shadowOffset());
	connect(m_shadow_offset, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	QHBoxLayout* color_layout = new QHBoxLayout;
	color_layout->setMargin(0);
	color_layout->addWidget(m_foreground_color);
	color_layout->addWidget(m_foreground_opacity);

	QHBoxLayout* size_layout = new QHBoxLayout;
	size_layout->setMargin(0);
	size_layout->addWidget(m_foreground_width);
	size_layout->addWidget(m_foreground_position);

	QFormLayout* foreground_layout = new QFormLayout;
	foreground_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	foreground_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	foreground_layout->setLabelAlignment(Qt::AlignRight);
	foreground_layout->addRow(tr("Color:"), color_layout);
	foreground_layout->addRow(tr("Size:"), size_layout);
	foreground_layout->addRow(tr("Rounding:"), m_foreground_rounding);
	foreground_layout->addRow(tr("Margin:"), m_foreground_margin);
	foreground_layout->addRow(tr("Padding:"), m_foreground_padding);

	QFormLayout* blur_layout = new QFormLayout(m_blur);
	blur_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	blur_layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
	blur_layout->setLabelAlignment(Qt::AlignRight);
	blur_layout->addRow(tr("Radius:"), m_blur_radius);

	QFormLayout* shadow_layout = new QFormLayout(m_shadow);
	shadow_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	shadow_layout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
	shadow_layout->setLabelAlignment(Qt::AlignRight);
	shadow_layout->addRow(tr("Color:"), m_shadow_color);
	shadow_layout->addRow(tr("Radius:"), m_shadow_radius);
	shadow_layout->addRow(tr("Vertical Offset:"), m_shadow_offset);

	QVBoxLayout* effects_layout = new QVBoxLayout;
	effects_layout->addWidget(m_blur);
	effects_layout->addSpacing(effects_layout->margin());
	effects_layout->addWidget(m_shadow);
	effects_layout->addStretch();

	QHBoxLayout* page_layout = new QHBoxLayout(tab);
	page_layout->addLayout(foreground_layout);
	page_layout->addSpacing(page_layout->margin() * 2);
	page_layout->addLayout(effects_layout, 1);


	// Create text group
	tab = new QWidget(this);
	tabs->addTab(tab, tr("Text"));

	m_text_color = new ColorButton(tab);
	m_text_color->setColor(m_theme.textColor());
	connect(m_text_color, SIGNAL(changed(QColor)), this, SLOT(renderPreview()));

	m_font_names = new FontComboBox(tab);
	m_font_names->setEditable(false);
	m_font_names->setCurrentFont(m_theme.textFont());
	connect(m_font_names, SIGNAL(activated(int)), this, SLOT(fontChanged()));
	connect(m_font_names, SIGNAL(activated(int)), this, SLOT(renderPreview()));

	m_font_sizes = new QComboBox(tab);
	m_font_sizes->setEditable(true);
	m_font_sizes->setMinimumContentsLength(3);
	connect(m_font_sizes, SIGNAL(editTextChanged(QString)), this, SLOT(renderPreview()));
	fontChanged();

	m_misspelled_color = new ColorButton(tab);
	m_misspelled_color->setColor(m_theme.misspelledColor());
	connect(m_misspelled_color, SIGNAL(changed(QColor)), this, SLOT(renderPreview()));

	QHBoxLayout* font_layout = new QHBoxLayout;
	font_layout->addWidget(m_font_names);
	font_layout->addWidget(m_font_sizes);

	QFormLayout* text_layout = new QFormLayout(tab);
	text_layout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	text_layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);
	text_layout->setLabelAlignment(Qt::AlignRight);
	text_layout->addRow(tr("Color:"), m_text_color);
	text_layout->addRow(tr("Font:"), font_layout);
	text_layout->addRow(tr("Misspelled:"), m_misspelled_color);


	// Create spacing group
	tab = new QWidget(this);
	tabs->addTab(tab, tr("Spacings"));

	QGroupBox* line_spacing = new QGroupBox(tr("Line Spacing"), tab);

	m_line_spacing_type = new QComboBox(line_spacing);
	m_line_spacing_type->setEditable(false);
	m_line_spacing_type->addItems(QStringList() << tr("Single") << tr("1.5 Lines") << tr("Double") << tr("Proportional"));
	m_line_spacing_type->setCurrentIndex(3);

	m_line_spacing = new QSpinBox(line_spacing);
	m_line_spacing->setSuffix(QLocale().percent());
	m_line_spacing->setRange(theme.lineSpacing().minimumValue(), theme.lineSpacing().maximumValue());
	m_line_spacing->setValue(m_theme.lineSpacing());
	m_line_spacing->setEnabled(false);

	switch (m_theme.lineSpacing()) {
	case 100: m_line_spacing_type->setCurrentIndex(0); break;
	case 150: m_line_spacing_type->setCurrentIndex(1); break;
	case 200: m_line_spacing_type->setCurrentIndex(2); break;
	default: m_line_spacing->setEnabled(true); break;
	}
	connect(m_line_spacing_type, SIGNAL(currentIndexChanged(int)), this, SLOT(lineSpacingChanged(int)));
	connect(m_line_spacing_type, SIGNAL(currentIndexChanged(int)), this, SLOT(renderPreview()));
	connect(m_line_spacing, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	QGroupBox* paragraph_spacing = new QGroupBox(tr("Paragraph Spacing"), tab);

	m_indent_first_line = new QCheckBox(paragraph_spacing);
	m_indent_first_line->setChecked(m_theme.indentFirstLine());
	connect(m_indent_first_line, SIGNAL(toggled(bool)), this, SLOT(renderPreview()));

	m_tab_width = new QSpinBox(paragraph_spacing);
	m_tab_width->setSuffix(tr(" pixels"));
	m_tab_width->setRange(theme.tabWidth().minimumValue(), theme.tabWidth().maximumValue());
	m_tab_width->setValue(m_theme.tabWidth());
	connect(m_tab_width, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_spacing_above_paragraph = new QSpinBox(paragraph_spacing);
	m_spacing_above_paragraph->setRange(theme.spacingAboveParagraph().minimumValue(), theme.spacingAboveParagraph().maximumValue());
	m_spacing_above_paragraph->setValue(m_theme.spacingAboveParagraph());
	connect(m_spacing_above_paragraph, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	m_spacing_below_paragraph = new QSpinBox(paragraph_spacing);
	m_spacing_below_paragraph->setRange(theme.spacingBelowParagraph().minimumValue(), theme.spacingBelowParagraph().maximumValue());
	m_spacing_below_paragraph->setValue(m_theme.spacingBelowParagraph());
	connect(m_spacing_below_paragraph, SIGNAL(valueChanged(int)), this, SLOT(renderPreview()));

	QHBoxLayout* line_spacing_layout = new QHBoxLayout(line_spacing);
	line_spacing_layout->addWidget(m_line_spacing_type);
	line_spacing_layout->addWidget(m_line_spacing);
	line_spacing_layout->addStretch();

	QFormLayout* paragraph_spacing_layout = new QFormLayout(paragraph_spacing);
	paragraph_spacing_layout->addRow(tr("Indent First Line:"), m_indent_first_line);
	paragraph_spacing_layout->addRow(tr("Tab Width:"), m_tab_width);
	paragraph_spacing_layout->addRow(tr("Pixels Above:"), m_spacing_above_paragraph);
	paragraph_spacing_layout->addRow(tr("Pixels Below:"), m_spacing_below_paragraph);

	QVBoxLayout* spacing_layout = new QVBoxLayout(tab);
	spacing_layout->addWidget(line_spacing);
	spacing_layout->addWidget(paragraph_spacing);
	spacing_layout->addStretch();

#if (QT_VERSION < QT_VERSION_CHECK(4,8,0))
	line_spacing->hide();
#endif


	// Create preview
	m_preview_text = new QTextEdit;
	m_preview_text->setFrameStyle(QFrame::NoFrame);
	m_preview_text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_preview_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_preview = new QLabel(this);
	m_preview->setAlignment(Qt::AlignCenter);
	m_preview->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

	QPixmap pixmap(480, 270);
	pixmap.fill(palette().window().color());
	m_preview->setPixmap(pixmap);

	renderPreview();


	// Lay out dialog
	QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_ok = buttons->button(QDialogButtonBox::Ok);
	connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

	QHBoxLayout* contents_layout = new QHBoxLayout;
	contents_layout->setMargin(0);
	contents_layout->addWidget(tabs);
	contents_layout->addWidget(m_preview, 0, Qt::AlignCenter);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(12);
	layout->addLayout(name_layout);
	layout->addLayout(contents_layout);
	layout->addWidget(buttons);
}

//-----------------------------------------------------------------------------

ThemeDialog::~ThemeDialog()
{
	delete m_preview_text;
}

//-----------------------------------------------------------------------------

void ThemeDialog::createPreview(const QString& name)
{
	Theme theme(name);
	ThemeDialog dialog(theme);
	dialog.savePreview();
}

//-----------------------------------------------------------------------------

void ThemeDialog::accept()
{
	m_theme.setName(m_name->text().simplified());
	setValues(m_theme);
	m_theme.saveChanges();

	savePreview();

	QDialog::accept();
}

//-----------------------------------------------------------------------------

void ThemeDialog::checkNameAvailable()
{
	QString name = m_name->text().simplified();
	bool empty = name.isEmpty();
	bool changed = (name != m_theme.name());
	bool exists = QFile::exists(Theme::filePath(name));
	m_ok->setEnabled(!changed || (!empty && !exists));
}

//-----------------------------------------------------------------------------

void ThemeDialog::fontChanged()
{
	QFontDatabase db;

	QFont font = m_font_names->currentFont();
	QList<int> font_sizes = db.smoothSizes(font.family(), QString());
	if (font_sizes.isEmpty()) {
		font_sizes = db.standardSizes();
	}
	qreal font_size = m_font_sizes->currentText().toDouble();
	if (font_size < 0.1) {
		font_size = qRound(m_theme.textFont().pointSizeF() * 10.0) * 0.1;
	}

	m_font_sizes->blockSignals(true);
	m_font_sizes->clear();
	int index = 0;
	for (int i = 0; i < font_sizes.count(); ++i) {
		int size = font_sizes.at(i);
		if (size <= font_size) {
			index = i;
		}
		m_font_sizes->addItem(QString::number(size));
	}
	m_font_sizes->setCurrentIndex(index);
	m_font_sizes->setEditText(QString::number(font_size));
	m_font_sizes->setValidator(new QDoubleValidator(font_sizes.first(), font_sizes.last(), 1, m_font_sizes));
	m_font_sizes->blockSignals(false);
}

//-----------------------------------------------------------------------------

void ThemeDialog::imageChanged()
{
	if (!m_background_image->image().isEmpty()) {
		if (m_background_type->currentIndex() == 0) {
			m_background_type->setCurrentIndex(5);
		}
	} else {
		m_background_type->setCurrentIndex(0);
	}
	renderPreview();
}

//-----------------------------------------------------------------------------

void ThemeDialog::lineSpacingChanged(int index)
{
	switch (index) {
	case 0:
		m_line_spacing->setValue(100);
		m_line_spacing->setEnabled(false);
		break;

	case 1:
		m_line_spacing->setValue(150);
		m_line_spacing->setEnabled(false);
		break;

	case 2:
		m_line_spacing->setValue(200);
		m_line_spacing->setEnabled(false);
		break;

	default:
		m_line_spacing->setEnabled(true);
		break;
	}
}

//-----------------------------------------------------------------------------

void ThemeDialog::renderPreview()
{
	m_clear_image->setEnabled(m_background_image->isEnabled() && !m_background_image->image().isEmpty());

	// Load theme
	Theme theme("", false);
	setValues(theme);
	theme.setBackgroundImage(m_background_image->image());

	// Reset text
	m_preview_text->clear();

	// Set colors
	QColor color = theme.foregroundColor();
	color.setAlpha(0);
	QColor text_color = theme.textColor();
	text_color.setAlpha(255);

	QPalette p = m_preview_text->palette();
	p.setColor(QPalette::Window, Qt::transparent);
	p.setColor(QPalette::Base, color);
	p.setColor(QPalette::Text, text_color);
	p.setColor(QPalette::Highlight, text_color);
	p.setColor(QPalette::HighlightedText, (qGray(text_color.rgb()) > 127) ? Qt::black : Qt::white);
	m_preview_text->setPalette(p);

	// Set spacings
	int tab_width = theme.tabWidth();
	QTextBlockFormat block_format;
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
	block_format.setLineHeight(theme.lineSpacing(), (theme.lineSpacing() == 100) ? QTextBlockFormat::SingleHeight : QTextBlockFormat::ProportionalHeight);
#endif
	block_format.setTextIndent(tab_width * theme.indentFirstLine());
	block_format.setTopMargin(theme.spacingAboveParagraph());
	block_format.setBottomMargin(theme.spacingBelowParagraph());
	m_preview_text->textCursor().mergeBlockFormat(block_format);
	m_preview_text->setTabStopWidth(tab_width);
	m_preview_text->document()->setIndentWidth(tab_width);

	// Set font
	m_preview_text->setFont(theme.textFont());
	m_preview_text->append(tr("The quick brown fox jumps over the lazy dog"));

	// Render theme
	QRect foreground;
	QImage preview = theme.render(QSize(1920, 1080), foreground);

	// Position preview text
	int padding = theme.foregroundPadding();
	int x = foreground.x() + padding;
	int y = foreground.y() + padding + theme.spacingAboveParagraph();
	int width = foreground.width() - (padding * 2);
	int height = foreground.height() - (padding * 2) - theme.spacingAboveParagraph();
	m_preview_text->setGeometry(x, y, width, height);

	// Render text
	m_preview_text->render(&preview, m_preview_text->pos());

	// Create preview pixmap
	preview = preview.scaled(480, 270, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	m_preview->setPixmap(QPixmap::fromImage(preview));
}

//-----------------------------------------------------------------------------

void ThemeDialog::savePreview()
{
	QPixmap preview(":/shadow.png");
	QPainter painter(&preview);
	painter.drawPixmap(9, 9, m_preview->pixmap()->scaled(240, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	preview.save(Theme::iconPath(m_theme.name()));
}

//-----------------------------------------------------------------------------

void ThemeDialog::setValues(Theme& theme)
{
	theme.setBackgroundType(m_background_type->currentIndex());
	theme.setBackgroundColor(m_background_color->color());
	theme.setBackgroundImage(m_background_image->toString());

	theme.setForegroundColor(m_foreground_color->color());
	theme.setForegroundOpacity(m_foreground_opacity->value());
	theme.setForegroundWidth(m_foreground_width->value());
	theme.setForegroundRounding(m_foreground_rounding->value());
	theme.setForegroundMargin(m_foreground_margin->value());
	theme.setForegroundPadding(m_foreground_padding->value());
	theme.setForegroundPosition(m_foreground_position->currentIndex());

	theme.setBlurEnabled(m_blur->isChecked());
	theme.setBlurRadius(m_blur_radius->value());
	theme.setShadowEnabled(m_shadow->isChecked());
	theme.setShadowColor(m_shadow_color->color());
	theme.setShadowRadius(m_shadow_radius->value());
	theme.setShadowOffset(m_shadow_offset->value());

	theme.setTextColor(m_text_color->color());
	QFont font = m_font_names->currentFont();
	font.setPointSizeF(m_font_sizes->currentText().toDouble());
	theme.setTextFont(font);
	theme.setMisspelledColor(m_misspelled_color->color());

	theme.setIndentFirstLine(m_indent_first_line->isChecked());
	theme.setLineSpacing(m_line_spacing->value());
	theme.setSpacingAboveParagraph(m_spacing_above_paragraph->value());
	theme.setSpacingBelowParagraph(m_spacing_below_paragraph->value());
	theme.setTabWidth(m_tab_width->value());
}

//-----------------------------------------------------------------------------
