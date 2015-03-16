# MVP
- render markdown by integrating https://github.com/zielmicha/markdown-to-terminal
   - add color/style
      - dividers and title bars in printed output
      - Highlight accepted answer
- wrap window/panel stuff in a special struct to bundle together
   - window and panel pointers
   - true original width/height
   - menu?
   - pad?
   - current scroll position (lets you return to where you left off)
   - desired current viewport?
- Remove either body or body_markdown from API filter to save bits on the wire
- full help msg for -h?
- check all TODOs/FIXMEs/XXXs



# Near Future
- switch to waddstr? instear of wprint? is there a benefit?
- Status bar in main window (for loading, progress, controls, etc)
   - callback to print in it too
- support 'h' for help (in status bar?)
- standardize error printing in stackexchange.c to use fprintf(stderr)
- use curl progress callbacks and curses to have a progress indicator (bar, or spinner, or total #)
   - pass callback after SEInit that calls into the display module to display progress
- show (and 1/2 cover) question content as the user scrolls through questions?
- Switch to bright UI colors like TIG
- redo question UI panel so it isnt a list of titles
   - want blurbs of each question body
- get more then 1 page of results from SE
   - user config? default #?
   - open UI immediately
   - request a small page first and post it to the UI for the user
   - as new pages come in, append them to the avalible questions list in the ui
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
- support SIGWINCH events (rescale UI)
- properly request gzipped content from SE https://api.stackexchange.com/docs/compression
- detect when we get decompressed content (can curl do this?) and dont error (do we now?)

# Far Future
- Offline SE
- ?
