// Scintilla header - Message definitions for text editor interface
// For full Scintilla documentation, see: https://www.scintilla.org/

#ifndef SCINTILLA_H
#define SCINTILLA_H

#define SCI_START 2000

// Scintilla messages (subset used by Notepad++)
#define SCI_ADDTEXT                 (SCI_START + 1)
#define SCI_ADDSTYLEDTEXT           (SCI_START + 2)
#define SCI_INSERTTEXT              (SCI_START + 3)
#define SCI_CLEARALL                (SCI_START + 4)
#define SCI_DELETERANGE             (SCI_START + 5)
#define SCI_CLEARDOCUMENTSTYLE      (SCI_START + 6)
#define SCI_GETLENGTH               (SCI_START + 7)
#define SCI_GETCHARAT               (SCI_START + 8)
#define SCI_GETCURRENTPOS           (SCI_START + 9)
#define SCI_GETANCHOR               (SCI_START + 10)
#define SCI_GETSTYLEAT              (SCI_START + 11)
#define SCI_REDO                    (SCI_START + 12)
#define SCI_SELECTALL               (SCI_START + 13)
#define SCI_SETSAVEPOINT            (SCI_START + 14)
#define SCI_GETSTYLEDTEXT           (SCI_START + 15)
#define SCI_CANREDO                 (SCI_START + 16)
#define SCI_MARKERLINEFROMHANDLE    (SCI_START + 17)
#define SCI_MARKERDELETEHANDLE      (SCI_START + 18)
#define SCI_GETVIEWEOL              (SCI_START + 19)
#define SCI_SETVIEWEOL              (SCI_START + 20)

#endif // SCINTILLA_H
