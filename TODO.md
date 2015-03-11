# MVP
- SEQuestion free fxn
- SEFreeQuery
- push pointer into SEQuestion type itself (need to make struct non-anon?)? Makes the API not so nasty. Could also typedef over it
- UI
   - Allow user to select question threads from search results to view
   - Display question and answers, sorted by rating
   - Highlight accepted answer
- Remove either body or body_markdown from API filter
- full help msg

# Near Future
- User config .file
   - Make it JSON so we can reuse the parser
   - Optional API/application key
   - Default sites to search
- Select other Stack Exchange sites
- Search multiple Stack Exchange sites
- Render pretty questions/answers
- Search query params
   - Sorting?
- Show more SE metadata (user ratings, etc)
- Allow more SE stuff
   - User lookup
   - Tag browsing

# Far Future
- Offline SE
- ?
