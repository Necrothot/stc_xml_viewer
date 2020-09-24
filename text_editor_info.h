#ifndef TEXT_EDITOR_INFO_H
#define TEXT_EDITOR_INFO_H

#include <QString>

class TextEditorInfo
{
public:
    TextEditorInfo(const QString &file_name);

    QString textEditor() const { return text_editor_; }
    QString fileFormats() const { return file_formats_; }
    QString encoding() const { return encoding_; }
    QString hasIntellisense() const { return has_intellisense_; }
    QString hasPlugins() const { return has_plugins_; }
    QString canCompile() const { return can_compile_; }

private:
    QString text_editor_;
    QString file_formats_;
    QString encoding_;
    QString has_intellisense_;
    QString has_plugins_;
    QString can_compile_;

};

#endif // TEXT_EDITOR_INFO_H
