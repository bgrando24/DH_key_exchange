# Diffie-Hellman Key Exchange (DHKE) implementation - Progress tracker

This document tracks and plans out the overall project for completing assessment 2.

This is intended not only for my own reference, but also for the teaching staff to gain a quick understanding of my current progress for this assignment.

Below is the currently planned stages/steps in order to complete this project, finished steps are marked as "[COMPLETED]".

# 1. Discovery and research [COMPLETED]

Key goals:

-   Understand the DHKE algorithm at a high level (be able to explain it to a 5 year old)
-   Understand what general features are required for the assignment demonstration
-   Ensure the specific assignment requirements are understood and aligned with the project's plan

---

# 2. Asio (C++ networking library) [COMPLETED]

(Asio) [https://think-async.com/Asio/] is a C++ library for networking. This is important since demonstrating the DHKE will require peer-to-peer communication, as well as demonstrating security against man-in-the-middle attacks.

Key goals:

-   Gain enough competency with the asio library to ensure the full demo can be built
-   Build a very simplified working proof-of-concept (C++ peer-to-peer or client-server communication)

---

# 3. Simplified DHKE proof-of-concept build

Key goals

-   Build a simplified proof-of-concept implementation for the DHKE algorithm
-   Demonstrate the main steps in DHKE: (Public key, first combined key exchange, second combined key exchange)

---

# 4. Refined DHKE build

Key goals:

-   Polish and refine the proof-of-concept build (ensure the full implementation is up to standard)
-   Ensure the specified security properties are demonstrated: confidentiality, integrity, authentication
-   Implement a man-in-the-middle attempted attack demonstration

---

# 5. Networking implementation

Key goals:

-   Integrate the refined DHKE implementation with the previously build asio networking functions/classes
-   Ensure communication acts as expected and can be observed cleanly for demonstration purposes

---

# 6. Implement a suitable encryption algorithm

Although the assignment focuses on the DHKE algorithm, I plan to polish off the project by actually using the established key in an encryption algorithm (e.g. AES, or other suitable algorithm).

Key goals:

-   Select a suitable encryption algorithm
-   Ensure that encryption using the key obtained from the DHKE can be demonstrated
