/*        Simple input

          A simple input handler that can tokenise input into short tokens
          (up to about 60 chars) by character class.
          A few common character class tests are provided, and the client
          can use their own.
*/






/// Character classes.

int IsAlpha       (char c) {c &= 0xDF; return c >= 'A'  &&  c <= 'Z';}
int IsNumeric     (char c) {return c >= '0' && c <= '9';}
int IsAlphaNumeric(char c) {return IsAlpha(c) || IsNumeric(c);}
int IsBlank       (char c) {return c == ' ';}
int IsEolnChar    (char c) {return c == 13  ||  c == 10;}
int NotEoln       (char c) {return c != 13  &&  c != 10;}
int IsOther       (char c) {return !IsAlphaNumeric(c) && !IsBlank(c) && NotEoln(c);}

/// Character classes end.






/// Circular input buffer.

char InputBuffer[256] = {0};
int  IIn              = 0;
int  IOut             = 0;
int  IEof             = 0;    // Input file has reached EOF (though there may still be data in the buffer)
int  IEoln            = 0;    // Last input file read ended in an eoln character

enum {ILimit = sizeof(InputBuffer)};

// in==out => empty. (i.e. Buffer may contan up to size-1 characters, it is never completely full.)
// in and out always wrap - they are never left at sizeof(InputBuffer).
// When all input removed, in and out are reset to 0.

int BufferContiguousFree() {
  if      (IIn < IOut) {return IOut   - IIn - 1;}
  else if (IOut > 0)   {return ILimit - IIn;}
  else                 {return ILimit - IIn - 1;}
}

void ContiguousFill() {
  int lengthRead = Read(Input, &InputBuffer[IIn], BufferContiguousFree());
  IEof  = (lengthRead == 0);
  IEoln = (lengthRead  &&  IsEolnChar(InputBuffer[IIn+lengthRead-1]));
  IIn   = (IIn + lengthRead) % ILimit;
}

int BufferTotalContent() {return (IIn + ILimit - IOut) % ILimit;}

void Fill() {
  // Note - if the last read ended in a newline, we don't read any more data
  // until this buffer is completely empty. This enables clients to display
  // a prompt before we read the next line.
  while (!IEof  &&  (!IEoln | (IIn==IOut))  &&  BufferTotalContent() < ILimit/2) {
    ContiguousFill();
  }
}

int Available() {Fill(); return BufferTotalContent();}

char NextCh() {return InputBuffer[IOut];}
void SkipCh() {IOut = (IOut + 1) % ILimit;}

void DumpInputState() {
  Ws("InputState: BufferTotalContent "); Wd(BufferTotalContent(),1);
  Ws(", IEof "); Wd(IEof,1);
  Ws(", IEoln "); Wd(IEoln,1);
  if (BufferTotalContent()) {
    Ws(", NextCh() '"); Wc(NextCh()); Wc('\'');
  }
  Wl();
}

/// Circular input buffer end.






/// Input parsing

int Eof()   {return Available() == 0;}
int Eoln()  {return Available() == 0  ||  IsEolnChar(NextCh());}

void ReadWhile(int (test)(char ch), char *buf, int bufLen) {
  Fill();
  while (buf < buf+bufLen-1  &&  IIn != IOut  &&  test(NextCh())) {
    *(buf++) = NextCh();
    SkipCh();
  }
  *buf = 0;
}

void SkipWhile(int (test)(char ch)) {
  Fill();
  while (IIn != IOut  &&  test(NextCh())) {SkipCh();}
}
void SkipEoln() {if (NextCh() == 13) {SkipCh();} if (NextCh() == 10) {SkipCh();}}

void Sa() {SkipWhile(IsAlpha);}
void Sb() {SkipWhile(IsBlank);}
void Sl() {SkipWhile(NotEoln); SkipEoln();}

void Ra (char *buf, int buflen) {ReadWhile(IsAlpha,        buf, buflen);}
void Rn (char *buf, int buflen) {ReadWhile(IsNumeric,      buf, buflen);}
void Ran(char *buf, int buflen) {ReadWhile(IsAlphaNumeric, buf, buflen);}
void Rb (char *buf, int buflen) {ReadWhile(IsBlank,        buf, buflen);}
void Ro (char *buf, int buflen) {ReadWhile(IsOther,        buf, buflen);}
void Rl (char *buf, int buflen) {ReadWhile(NotEoln,        buf, buflen); Sl();}

// Input parsing end.
