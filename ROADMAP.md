# Roadmap

A learning project in C: a console Q&A program that answers questions it knows,
learns new answers from an admin, and remembers them across runs.

## Goal
The program answers a question if it has the answer. If not, the question is saved
as a suggestion. Only admins can add, delete, or update records; regular users can
only ask questions.

## Steps

1. **[DONE] Input trimming**: Extra spaces (e.g. `"42+9 "`) no longer break matching.
2. **[DONE] Persistence**: Question/answer pairs are stored in a file (question line,
   then answer line) using `fopen`/`fgets`/`fputs`.
3. **[SKIPPED] `list` command**: Not useful once the record file grows large.
4. **[DONE] Arithmetic**: Expressions like `3+5` are parsed with `sscanf` and calculated.
5. **[SKIPPED] Dynamic array**: Records are searched directly in the file, so
   `malloc`/`realloc` is not needed.
6. **[DONE] Login and permissions (local prototype)**: Admins log in with a username
   and password from the `yetkililer` file. Not real security yet; moved to the
   server in step 10.
7. **[DONE] `unut` (forget) command, admin only**: Copies every record except the
   deleted one into a temporary file, then replaces the original with `rename`
   (safer than `remove` + `rename`).
8. **[DONE] Answer update, admin only**: Same copy technique; the matching answer is
   replaced instead of skipped.
9. **[POSTPONED] Suggestions and admin approval**: Unknown questions are already written
   to a suggestions file. Admin approval will come after 13.2.
10. **[IN PROGRESS] Client–server split (real protection)**: Records live on a server;
    users connect with a client over POSIX sockets.
    - 10.1 **[DONE]** Separate logic from I/O: an `isle(session, input, output)`
      function and a session struct holding login state.
    - 10.2 **[IN PROGRESS]** Learn sockets with a separate echo server/client.
    - 10.3 Line protocol: TCP is a byte stream, so a helper reads until `\n`.
    - 10.4 Connect: server receives a line → `isle()` → sends the reply.
    - 10.5 Move login and admin state to the server, per connection.
    - 10.6 Multiple clients (`fork`, then `select`/threads); handle concurrent file writes.
11. **(Optional) Real database**: SQLite instead of the record file.
12. **Fetching information from the internet**: libcurl and JSON parsing, learned first
    in a separate small project.
13. **"Thinking"**
    - 13.1 **[DONE]** Normalization: `"Ankara kaç harfli?"` and `" ankara kaç harfli"`
      count as the same question (case, punctuation, extra spaces, Turkish UTF-8 characters).
    - 13.2 Generate answers for suggested questions from rules, the internet, or an AI
      model (depends on 12). Answers are drafts that an admin approves in step 9.
