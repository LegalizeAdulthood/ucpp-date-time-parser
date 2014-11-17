This is an example of a recursive descent [RFC 5322 date](http://tools.ietf.org/html/rfc5322) parser written using
[Boost.Spirit](http://www.boost.org/doc/libs/1_57_0/libs/spirit/doc/html/index.html).

This parser was developed incrementally, commit by commit.  Browse the tags
to see how this parser evolved from simple cases up to handling the basic
RFC 5322 date grammar.

After handling the basics, validation of the input was added.

Finally, parsing the "obsolete" elements of RFC 5322 was added.

Tips
====
Here are some tips for creating parsers with Spirit:

- Develop your parsers incrementally, starting with the smallest parsable
  unit.  Drive your parser forward step-by-step with unit tests to keep
  everything working as you enhance the parser.
- When building up parser rules and grammars driven by tests:
  - Use `lit("expected input text")` and `attr()` to match the input of
    test cases and supply the necessary attribute values.  As you add more
    test cases for your grammar, you will drive out these intermediate
    steps with parser details.
  - Start with the simplest cases that omit all optional elements of your
    grammar.
  - Start with a single choice among alternatives in your rules.  Use
    additional tests to drive the remaining alternatives into your grammar.
    This keeps your grammar from adding too many new rules and non-terminals
    in a single step, keeping it easy to debug.
  - Extract grammar rules incrementally as you build up functionality.
    Refactor out rules in your grammar after passing new test cases.
- Remember that rule and grammar attribute types are specified by function signatures:
  write `rule<Iter, bool(), skipper> boolean`, not `rule<Iter, bool, skipper> boolean`.
- Order the elements of a struct in `BOOST_FUSION_ADAPT_STRUCT` so that it is convenient
  for your parser attribute sequence.
- When using `BOOST_FUSION_ADAPT_STRUCT`, make sure that your grammar generates attributes
  for each adapted struct member.
- Use the `&` parser (and predicate) when you require a prefix to exist in the
  input stream, but want those characters to be consumed by a subsequent
  parser.  For example, `&(lit('+') | '-') >> int_` requires an integer
  quantity to have a sign prefix, but leaves that prefix in place for `int_`
  to consume so that the attribute value is appropriately signed.
- When you get a strange compile error in a Spirit header, look for a comment
  in the header that explains the most likely cause of the compile error and
  what to do about it.  The Spirit library authors have tried to anticipate
  common mistakes to avoid "template compile error hell". 
- Spirit parsers leverage templates heavily to achieve fast runtime at the expense of
  compile time.  Isolate your parsers behind an application specific API.  The parser
  implementation only needs to be recompiled when the parser changes.  The parser can
  be reused in as many places as possible without recompiling the parser.

RFC 5322 Date Productions
=========================
```
   date-time       =   [ day-of-week "," ] date time [CFWS]

   day-of-week     =   ([FWS] day-name) / obs-day-of-week

   day-name        =   "Mon" / "Tue" / "Wed" / "Thu" /
                       "Fri" / "Sat" / "Sun"

   date            =   day month year

   day             =   ([FWS] 1*2DIGIT FWS) / obs-day

   DIGIT           =   %x30-39  ; 0-9

   month           =   "Jan" / "Feb" / "Mar" / "Apr" /
                       "May" / "Jun" / "Jul" / "Aug" /
                       "Sep" / "Oct" / "Nov" / "Dec"

   year            =   (FWS 4*DIGIT FWS) / obs-year

   time            =   time-of-day zone

   time-of-day     =   hour ":" minute [ ":" second ]

   hour            =   2DIGIT / obs-hour

   minute          =   2DIGIT / obs-minute

   second          =   2DIGIT / obs-second

   zone            =   (FWS ( "+" / "-" ) 4DIGIT) / obs-zone

   FWS             =   ([*WSP CRLF] 1*WSP) /  obs-FWS
                                          ; Folding white space

   CFWS            =   (1*([FWS] comment) [FWS]) / FWS

   comment         =   "(" *([FWS] ccontent) [FWS] ")"

   ccontent        =   ctext / quoted-pair / comment

   ctext           =   %d33-39 /          ; Printable US-ASCII
                       %d42-91 /          ;  characters not including
                       %d93-126 /         ;  "(", ")", or "\"
                       obs-ctext

   quoted-pair     =   ("\" (VCHAR / WSP)) / obs-qp

   VCHAR           =   %x21-7E
                       ; visible (printing) characters

   WSP             =   SP / HTAB
                       ; white space

   SP              =   %x20

   HTAB            =   %x09     ; horizontal tab

   CRLF            =   CR LF    ; Internet standard newline

   LF              =   %x0A     ; linefeed

   CR              =   %x0D     ; carriage return

   obs-day-of-week =   [CFWS] day-name [CFWS]

   obs-day         =   [CFWS] 1*2DIGIT [CFWS]

   obs-year        =   [CFWS] 2*DIGIT [CFWS]

   obs-hour        =   [CFWS] 2DIGIT [CFWS]

   obs-minute      =   [CFWS] 2DIGIT [CFWS]

   obs-second      =   [CFWS] 2DIGIT [CFWS]

   obs-zone        =   "UT" / "GMT" /     ; Universal Time
                                          ; North American UT
                                          ; offsets
                       "EST" / "EDT" /    ; Eastern:  - 5/ - 4
                       "CST" / "CDT" /    ; Central:  - 6/ - 5
                       "MST" / "MDT" /    ; Mountain: - 7/ - 6
                       "PST" / "PDT" /    ; Pacific:  - 8/ - 7
                                          ;

   obs-ctext       =   obs-NO-WS-CTL

   obs-NO-WS-CTL   =   %d1-8 /            ; US-ASCII control
                       %d11 /             ;  characters that do not
                       %d12 /             ;  include the carriage
                       %d14-31 /          ;  return, line feed, and
                       %d127              ;  white space characters

   obs-qp          =   "\" (%d0 / obs-NO-WS-CTL / LF / CR)

   obs-FWS         =   1*WSP *(CRLF 1*WSP)
```
