# MVP
- TODO: WAS WORKING ON: I think the subpad stuff is broken. is my derwin ok?
- make each panel scroll if its selected
- when user selects a question, switch to answer panel and populate it with answers
- DO NOT select quesions as the user pages through them? how to we switch focus to the other window?
- special fxns for printing question bodies?
   - remove \n\r's and replace with curses move operations?
   - remove html tags?
- any reason to use waddstr? instear of wprint?
- UI
   - Allow user to select question threads from search results to view
   - Display question and answers, sorted by rating
   - Highlight accepted answer
- Remove either body or body_markdown from API filter to save bits
- sort answers (accepted, then votes)
- full help msg
- remove random printing in main/stackexchange/etc
- check all TODOs/FIXMEs/XXXs


# Near Future
- use curl progress callbacks and curses to have a progress indicator (bar, or spinner, or total #)
   - pass callback after SEInit that calls into the display module to display progress
- Switch to bring UI colors like TIG
- redo question UI panel so it isnt a list of titles
   - want blurbs of each question body
- get more then 1 page of results from SE
   - user config? default #?
- support more vi page controls
- support SIGWINCH events (rescale UI)
- User config .file
   - Make it JSON so we can reuse the parser
   - Optional API/application key
   - Default sites to search
- Select other Stack Exchange sites
- Separate SE query from UI. Draw UI immediately while beginning to make API requests
- Search multiple Stack Exchange sites
- Render prettier questions/answers
- Search query params
   - Sorting?
- Show more SE metadata (user ratings, etc)
- Allow more SE stuff
   - User lookup
   - Tag browsing

# Far Future
- Offline SE
- ?
