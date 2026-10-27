# basic mind

A small question-and-answer console program written in C, built as a learning project.

If it knows the answer to a question, it replies. If it doesn't, the question is saved
as a suggestion, and an admin can teach it the answer. The program remembers what it
learns across runs by storing records in a plain text file.

It doesn't "think". It just matches questions to stored answers. The point of the project
is to practice C step by step: file I/O, strings, structs, pointers, and later sockets.

## Features

- Answers questions it has learned before
- Treats similar questions as the same (case, punctuation, and extra spaces are normalized,
  including Turkish characters)
- Calculates simple arithmetic such as `12 + 7` or `9/3`
- Admin login for teaching (`admin`), deleting (`unut`), and updating (`güncelle`) answers
- Regular users can only ask; unknown questions go to a suggestions file

## Build and run

Requires a C compiler (gcc) on Linux.

    gcc -Wall test_updated.c -o test_updated
    ./test_updated

Type a question and press Enter. Type `çık` or press Ctrl+D to quit.

## Setting up an admin

Admin credentials are not included in the repository. Create a file named `yetkililer`
next to the program, with two lines per admin: the username, then the password.

    myname
    mypassword

Without this file the program still runs, but nobody can log in as admin.

## Commands

| Command    | Who      | What it does                          |
|------------|----------|---------------------------------------|
| `admin`    | anyone   | Log in as admin                       |
| `unut`     | admin    | Delete a question and its answer      |
| `güncelle` | admin    | Change the answer to a question       |
| `çık`      | anyone   | Quit                                  |

## Files

| File              | Description                                             |
|-------------------|---------------------------------------------------------|
| `test_updated.c`  | Current version of the program                          |
| `test_old.c`      | Earlier version, kept for reference                     |
| `ROADMAP.md`      | Project goals and progress                              |
| `kayıt dosyası`   | Learned questions and answers (created at runtime)      |
| `öneriler`        | Unknown questions suggested by users (created at runtime) |
| `yetkililer`      | Admin credentials (you create this; not in the repo)    |

## Status

Work in progress. The next step is a client–server version using POSIX sockets, so the
records and admin checks live on a server instead of the user's machine. See
[ROADMAP.md](ROADMAP.md).
