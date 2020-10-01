## STC XML Viewer

#### Overview

Demo project of GUI application for viewing and editing SQLite database of listed text editor's properties:

1. `texteditor` - name
2. `fileformats` - supported file formats
3. `encoding` - supported encodings
4. `hasintellisense` - does it has *IntelliSense*?
5. `hasplugins` - does it has plugins?
6. `cancompile` - can it compile code?

To populate database, click on **Open** button and select folder with `.xml` files with above mentioned properties or choose **Add row** from context menu

All table cells are editable. Just highlight a cell and type in desired value

To clear database, press **Clear DB** button. For removing particular row, open context menu on it and select **Remove row**

Table's row exporting can be performed by selecting **Save to file** from context menu. It will bring file dialog for choosing output XML file

#### TODO

1. Execute SQL queries in different thread
2. Refactor **TextEditorModel** implementation
3. Complete code documentation