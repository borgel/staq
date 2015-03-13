# staq
A Stack Exchange question/answer browser for a commandline world.

# Usage
To search for "c ncurses window splitting" do this:
```
staq c ncurses window splitting
```

# Why?
When programming, often I use two tools:
- My terminal
- A browser, open to Stack Overflow to solve problems

It would be ideal to fulfull both of these purposes using a purely console-based interface.

Inspired by `tig`, an excellent git log browser.

# Installation
You will need...
- libcurl
- jansson (a C JSON lib)

# Comments
The Stack Exchange interface code is written to be more or less standalone, so it could be easily integrated into other projects. The curses display code... not so much. Ultimately I decided it was application-specific enough to not bother generalizing.

## Some Sample Stack Exchange Queries

'c string concatenation', sorted by descending relevance. Filter showing body and body in markdown of all questions.
```
https://api.stackexchange.com/docs/advanced-search#order=desc&sort=relevance&q=c%20string%20concatenation&filter=!bJDus*tEQj87Wy&site=stackoverflow&run=true
https://api.stackexchange.com/2.2/search/advanced?order=desc&sort=relevance&q=c%20string%20concatenation&site=stackoverflow&filter=!4*SyY(M(4WWPiOhna
```

All answers to question ID 9555167. With body and body in markdown. Sorted by _votes_. The accepted answer is marked with the property `is_accepted: true`, but would need to be manually highlighted.
```
https://api.stackexchange.com/docs/answers-on-questions#order=desc&sort=votes&ids=9555167&filter=!9YdnSMldD&site=stackoverflow&run=true
https://api.stackexchange.com/2.2/questions/9555167/answers?order=desc&sort=votes&site=stackoverflow&filter=!9YdnSMldD
```

