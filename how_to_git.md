Prompt:

now, for the first time, i will be working on a project (minishell) together with a peer (Juliyan), using one common repository. what do we need to know, how do we prevent breaking stuff accidentially?


Chats Answer:

Great question — this is **exactly** the right moment to set good habits. Minishell + first shared repo is where many 42 projects go wrong *socially*, not technically 🙂

I’ll explain this **conceptually first (no commands)**, then give you a **very small, safe workflow** that prevents 90% of accidents.

---

## 1️⃣ One shared repo = shared responsibility

When you and Juliyan work on **one repository**:

* Every `git push` changes the *same* codebase
* You can overwrite each other’s work **without noticing**
* Git will not protect you from bad decisions — only from conflicts it detects

So the goal is:

> **Never push directly to shared main code without context**

---

## 2️⃣ Golden rule (this saves friendships)

**Never both work directly on `main` (or `master`).**
Even if Git allows it.

Instead:

* `main` = **stable, compiling, working code**
* All work happens **elsewhere**
* Only *finished + tested* code goes back to `main`

---

## 3️⃣ Mental model: “parallel universes”

Think of Git like this:

* `main` → *the official universe*
* branches → *parallel universes*
* merge → *carefully combining realities*

You and Juliyan should almost **never be in the same universe while coding**.

---

## 4️⃣ Minimal safe workflow (42-friendly)

You only need **three rules**.

### Rule 1: One branch per person or per task

Examples:

* `maria-parsing`
* `juliyan-exec`
* `signals`
* `builtin-cd`

This means:

* You can break things freely
* You cannot break *each other*

---

### Rule 2: Small, focused commits

A commit should answer:

> *“What did this change do?”*

Good:

* “Implement token struct”
* “Add env list utilities”
* “Fix memory leak in parser”

Bad:

* “work”
* “stuff”
* “fix”

Small commits make:

* debugging easier
* conflicts smaller
* evaluations calmer

---

### Rule 3: Merge only when code compiles

Before merging into `main`:

* `make`
* basic tests
* no obvious leaks
* no half-finished functions

If something is **unfinished**, it stays in the branch.

---

## 5️⃣ How conflicts actually happen (important)

Conflicts usually occur when:

* both of you edit **the same file**
* and Git can’t auto-merge

Typical conflict hotspots in minishell:

* `minishell.h`
* shared structs
* global variables
* Makefile

💡 **Prevent conflicts by agreeing early**:

* Who owns which files?
* Who edits headers?
* Who touches Makefile?

---

## 6️⃣ Very important: TALK before structural changes

Before anyone:

* renames files
* changes structs
* changes function prototypes
* touches headers used everywhere

→ **tell the other person first**

This avoids:

* massive conflicts
* silent breakage
* “it worked on my branch” moments

---

## 7️⃣ Practical division of labor (recommended for minishell)

A classic, low-conflict split:

**Maria**

* parsing
* tokenization
* syntax checks
* env handling

**Juliyan**

* execution
* pipes
* redirections
* builtins

Shared:

* signals
* error handling
* memory cleanup

This reduces overlap drastically.

---

## 8️⃣ Safety net habits (do these always)

Before pulling or merging:

* commit your work (even if messy)
* never pull with uncommitted changes

If something breaks:

* Git history is your parachute
* You can always go back

---

## 9️⃣ Evaluation mindset (this matters at 42)

Evaluators care that:

* both of you understand the whole project
* Git history is readable
* main branch is clean
* no “last minute chaos commits”

A calm Git history = strong signal.

---

## 10️⃣ Reassurance

You **will**:

* break the build at some point
* create a conflict
* push something imperfect once

That’s normal.

What matters is:

* you notice early
* you talk
* you fix together

