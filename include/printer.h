#ifndef __PRINTER_H
#define __PRINTER_H

// --------------------------------
class Printer {
public:
  virtual void header() {}
  virtual void odds(int income, int cost, int x, int y, bool percent, float mul, float thr) = 0;
  virtual void footer() {}
};

class PrinterXterm : public Printer {
  virtual void odds(int income, int cost, int x, int y, bool percent, float mul, float thr);
};

class PrinterHtml : public Printer {
  virtual void header();
  virtual void footer();
  virtual void odds(int income, int cost, int x, int y, bool percent, float mul, float thr);

#if 0
  void header() {
    printf("<html><body>\n<pre style='color:#ccc;background:#000;padding:4px;font:12px \"Lucida Console\"'>");
  }

  void footer() {
    printf("</pre></body></html>");
  }
#endif
};

extern Printer *printer;

#endif //__PRINTER_H
