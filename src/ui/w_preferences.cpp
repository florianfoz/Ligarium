#include "widget/w_preferences.h"

#include "base.h"
#include "csshighlighter.h"
#include "database/database.h"
#include "database/manager.h"
#include "ui_w_preferences.h"
#include "widget/database/w_database_creator.h"
#include "widget/w_preferences.h"

#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>

W_Preferences::W_Preferences(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::W_Preferences)
{
  ui->setupUi(this);

  auto* db_creator = new W_Database_Creator(this, Database_Manager::current_database()->folder_path());
  ui->layout_db_settings->addWidget(db_creator);

  db_creator->remove_dialog_buttons();

  high = new CssHighlighter(ui->te_custom_theme_CSS->document());

  Preferences::load();

  ui->cb_language->clear();
  ui->cb_theme->clear();

  QStringList software_ts = QDir(":/translations/").entryList(QDir::Files);
  for (auto& t : software_ts) {
    QString loc = getLocaleFromFile(t);
    QLocale iso(loc);

    ui->cb_language->addItem(iso.nativeLanguageName() + ", " + iso.nativeTerritoryName(), iso.name());
  }

  QStringList user_ts = QDir(TRADUCTION_PATH() + "/user").entryList(QStringList() << "*.qm", QDir::Files);
  for (auto& t : user_ts) {
    QString loc = getLocaleFromFile(t);
    QLocale iso(loc);

    ui->cb_language->addItem(iso.nativeLanguageName() + ", " + iso.nativeTerritoryName(), iso.name());
  }

  int lang_index = ui->cb_language->findData(Preferences::locale().name());
  ui->cb_language->setCurrentIndex(lang_index);

  ui->cb_theme->addItem("Rentalis - Standard", "rentalis-standard");
  ui->cb_theme->addItem("Rentalis - Light", "rentalis-light");
  ui->cb_theme->addItem("Rentalis - Dark", "rentalis-dark");
  ui->cb_theme->addItem("Custom", "custom");

  QStringList user_th = QDir(THEME_PATH() + "/user").entryList(QStringList() << "*.css", QDir::Files);
  for (const auto& t : user_th) {
    ui->cb_theme->addItem("user/" + t, t);
  }

  int theme_index = ui->cb_theme->findData(Preferences::theme);
  ui->cb_theme->setCurrentIndex(theme_index);

  ui->te_custom_theme_CSS->setText(Preferences::custom_theme);

  ui->cbox_autosave_activate->setChecked(Preferences::autosave);
  ui->cbox_autosave_override->setChecked(Preferences::autosave_override);
  ui->ds_autosave_frequency->setValue(Preferences::autosave_frequency_minutes);
}

W_Preferences::~W_Preferences()
{
  delete ui;
}

void W_Preferences::update_settings()
{
  Preferences::ask_ai_url                 = ui->le_ask_ai_url->text();
  Preferences::language                   = ui->cb_language->currentData().toString();
  Preferences::theme                      = ui->cb_theme->currentData().toString();
  Preferences::custom_theme               = ui->te_custom_theme_CSS->toPlainText();
  Preferences::autosave                   = ui->cbox_autosave_activate->isChecked();
  Preferences::autosave_override          = ui->cbox_autosave_override->isChecked();
  Preferences::autosave_frequency_minutes = ui->ds_autosave_frequency->value();

  qApp->setStyleSheet(Preferences::theme_css());

  Preferences::save();
}


void W_Preferences::on_buttonBox_accepted()
{
  update_settings();

  close();
}


void W_Preferences::on_buttonBox_rejected()
{
  close();
}


void W_Preferences::on_b_ask_to_ai_url_help_clicked()
{
  QString msg =
      tr("<b>Usage Instructions for AI Prompt URL Field</b><br>"
         "In this field, you can enter a URL template for generating prompts with an AI service. Use the placeholders "
         "below to customize the URL:"
         "<ul>"
         "<li><b>%1</b> – This placeholder will be replaced by the actual prompt text you want to send.</li>"
         "<li><b>%2</b> – This placeholder can be used to set the value of the <code>submit</code> parameter if the AI "
         "service supports automatic submission.</li>"
         "</ul>"
         "Example URL template:"
         "<pre>https://your-ai-instance.com/c/new?prompt=%1&submit=%2</pre>"
         "This template allows you to quickly generate URLs for testing or automating prompt submissions while keeping "
         "the prompt dynamic.");

  QMessageBox::information(this, tr("How to use a AI Prompt URL"), msg);
}


void W_Preferences::on_b_reset_theme_clicked()
{
  ui->te_custom_theme_CSS->setText(THEME_LIGHT_CSS);
  qApp->setStyleSheet(THEME_LIGHT_CSS);
}


void W_Preferences::on_buttonBox_clicked(QAbstractButton* button)
{
  if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
    update_settings();
  } else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole) {
    update_settings();
    close();
  } else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::RejectRole) {
    close();
  }
}


void W_Preferences::on_cb_theme_currentIndexChanged(int index)
{
  ui->gb_custom_theme->setHidden(ui->cb_theme->currentData().toString() != "custom");
}


void W_Preferences::on_b_theme_folder_clicked()
{
  auto f = QFileInfo(THEME_PATH());
  QDesktopServices::openUrl(QUrl::fromLocalFile(f.absolutePath()));
}
