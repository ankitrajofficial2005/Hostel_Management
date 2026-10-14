# Hostel Management System (DSA in C)

A complete, production-grade, terminal-based **Hostel Management System** implemented strictly in **C (C11 standard)** using fundamental **Data Structures and Algorithms (DSA)**.

Built for college academic submissions (B.Tech Computer Science / Information Technology / MCA), this project demonstrates practical applications of **Singly Linked Lists**, **Arrays**, **FIFO Queues**, and **LIFO Stacks** alongside robust file persistence and terminal UI design.

---

## 📋 Table of Contents

1. [Key Features](#-key-features)
2. [Data Structures & Algorithms Overview](#-data-structures--algorithms-overview)
3. [Time & Space Complexity Analysis](#-time--space-complexity-analysis)
4. [Project Structure](#-project-structure)
5. [Compilation & Execution](#-compilation--execution)
6. [Menu Options Breakdown](#-menu-options-breakdown)
7. [Data Persistence Architecture](#-data-persistence-architecture)
8. [Sample Terminal Output](#-sample-terminal-output)
9. [Viva / Presentation Guide](#-viva--presentation-guide)
10. [Completed & Tested Features Checklist](#-completed--tested-features-checklist)

---

## 🌟 Key Features

- **Strict Pure C Implementation**: Uses only standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `time.h`). Zero external dependencies or GUI frameworks.
- **Dynamic Student Management**: Singly linked list with dynamic heap memory allocation (`malloc`, `free`), duplicate ID protection, and search/update/delete capabilities.
- **O(N log N) Sorting**: Implementation of **Merge Sort** directly on the singly linked list for sorting by Student ID and alphabetical Student Name.
- **Room Allocation & Capacity Control**: Array of room structures tracking multi-occupancy shared rooms, dynamic occupancy calculations, and status progression (`AVAILABLE` $\to$ `PARTIAL` $\to$ `FULL`).
- **Waiting List FIFO Queue**: Linked-node queue ensuring fair accommodation queuing when rooms are fully booked. Automatic vacancy detection triggers room offers to waiting students upon room checkout.
- **Administrative Action Stack (Audit Log)**: LIFO stack tracking administrative operations (registration, allocation, vacating, fee payment) for audit and history tracking.
- **Hostel Fee Accounting**: Computes pending balances (`Total Fee - Amount Paid`), validates payments, rejects negative/excess payments, and generates financial summaries.
- **Safe File Persistence**: Plain-text pipe-delimited storage for student records, room configurations, and waiting lists. Synchronizes room occupancy counts against active student records on startup.
- **Defensive Input Handling**: Safe buffer inputs with `fgets()`, line trimming, input type checks, range validation, and confirmation prompts for destructive actions.

---

## 🧬 Data Structures & Algorithms Overview

| Module | Primary Data Structure | Reason for Selection | Key Operations |
|---|---|---|---|
| **Student Management** | **Singly Linked List** (`Student*`) | Dynamic size flexibility; efficient $O(1)$ insertions without memory re-allocation overheads. | Insert, Linear Search, In-place Update, Node Deletion, Pointer Traversal |
| **Sorting** | **Merge Sort on Linked List** | Optimal $O(N \log N)$ comparison sort that does not require random access (ideal for linked lists). | Slow/Fast pointer split (`split_list`), Recursive Merge (`merge_sort`) |
| **Room Management** | **Fixed/Configurable Array** (`Room[]`) | Contiguous memory allocation, $O(1)$ indexed access for predictable hostel layouts and room floors. | Allocation, Vacating, Linear Scan for Availability, Occupancy Sync |
| **Waiting List** | **Linked FIFO Queue** (`WaitingQueue`) | Strict First-In First-Out fairness; dynamic node growth without fixed capacity constraints. | `enqueue` at rear, `dequeue` from front, `peek`, `remove_by_id` |
| **Audit Log** | **Linked LIFO Stack** (`ActionStack`) | Last-In First-Out chronological action tracking; newest administrative event sits at the top. | `push`, `pop`, bounded capacity pruning |
| **Storage Persistence** | **Delimited File Streams** (`FILE*`) | Portable, human-readable records without proprietary binary formatting or raw pointer dumps. | Pipe-delimited parsing, directory creation, sync verification |

---

## ⏱️ Time & Space Complexity Analysis

| Operation | Data Structure | Time Complexity (Best) | Time Complexity (Average) | Time Complexity (Worst) | Auxiliary Space |
|---|---|---|---|---|---|
| **Add Student** | Linked List | $O(1)$ | $O(N)$ (due to unique ID check) | $O(N)$ | $O(1)$ |
| **Search Student by ID** | Linked List | $O(1)$ (head match) | $O(N)$ | $O(N)$ | $O(1)$ |
| **Delete Student Record**| Linked List | $O(1)$ (head) | $O(N)$ | $O(N)$ | $O(1)$ |
| **Sort by ID / Name** | Merge Sort on List | $O(N \log N)$ | $O(N \log N)$ | $O(N \log N)$ | $O(\log N)$ (call stack) |
| **Find Available Room** | Room Array | $O(1)$ | $O(M)$ ($M$ = room count) | $O(M)$ | $O(1)$ |
| **Allocate / Vacate Room**| Room Array | $O(1)$ | $O(M)$ | $O(M)$ | $O(1)$ |
| **Enqueue Waiting Student**| Linked Queue | $O(1)$ | $O(K)$ (duplicate check) | $O(K)$ ($K$ = queue size) | $O(1)$ |
| **Dequeue Waiting Student**| Linked Queue | $O(1)$ | $O(1)$ | $O(1)$ | $O(1)$ |
| **Push Audit Action** | Linked Stack | $O(1)$ | $O(1)$ | $O(1)$ | $O(1)$ |
| **Pop Audit Action** | Linked Stack | $O(1)$ | $O(1)$ | $O(1)$ | $O(1)$ |
| **Sync Room Occupancy** | Array + List | $O(N \times M)$ | $O(N \times M)$ | $O(N \times M)$ | $O(1)$ |

---

## 📂 Project Structure

```
Hostel_management/
├── main.c              # Application entry point, main loop, 16 menu handlers
├── student.h           # Student structure declaration and linked list API
├── student.c           # Linked list implementation & O(N log N) Merge Sort
├── room.h              # Room structure, RoomStatus enum, room array API
├── room.c              # Room allocation, vacating, and occupancy sync logic
├── queue.h             # FIFO Waiting list queue node and structure API
├── queue.c             # FIFO Queue implementation (enqueue, dequeue, search)
├── stack.h             # LIFO Action history stack declarations
├── stack.c             # Action history stack implementation (push, pop, prune)
├── storage.h           # File persistence prototypes and paths
├── storage.c           # Pipe-delimited file reading, writing, and directory initialization
├── utils.h             # Safe input helpers and formatting utilities
├── utils.c             # String trimming, range-checked input, timestamping
├── test_dsa.c          # Automated test suite verifying data structures in isolation
├── test_interactive.sh # Automated end-to-end interactive terminal test runner
├── Makefile            # Build configuration for GCC / Clang
├── README.md           # Documentation, architectural guide, and viva tips
└── data/               # Auto-created persistence directory
    ├── students.txt    # Pipe-delimited student records
    ├── rooms.txt       # Room capacity and occupancy state
    └── waiting_list.txt# Queued students awaiting room vacancy
```

---

## 🛠️ Compilation & Execution

### Prerequisites
- Operating System: macOS or Linux (Ubuntu, Debian, Fedora, Arch, etc.)
- Compiler: GCC (version 4.9+) or Clang (LLVM) supporting the **C11 standard**
- Make utility

### Compile the Application
```bash
make clean && make
```

### Run the Application
```bash
./hostel_mgmt
# or simply:
make run
```

### Run the Automated Unit Test Suite
```bash
make test
```
The test suite runs 70 automated assertions covering all linked list operations, merge sort, queue FIFO ordering, stack LIFO behavior, room allocation/vacating rules, and data persistence reloading.

### Run Automated End-to-End Terminal Flow Test
```bash
chmod +x test_interactive.sh
./test_interactive.sh
```

---

## 📋 Menu Options Breakdown

The application features an interactive terminal interface with 16 menu options:

```
+===================================================================+
|                    HOSTEL MANAGEMENT SYSTEM                       |
|                DSA Project - C11 Standard Library                 |
+===================================================================+
|  [1]  Add New Student            |  [9]  Display Available Rooms  |
|  [2]  Display All Students       |  [10] Manage Hostel Fees       |
|  [3]  Search Student by ID       |  [11] Students with Pending Fee|
|  [4]  Update Student Details     |  [12] Manage Waiting List      |
|  [5]  Delete Student Record      |  [13] Sort Students by ID      |
|  [6]  Allocate Hostel Room       |  [14] Sort Students by Name    |
|  [7]  Vacate Hostel Room         |  [15] Display Hostel Statistics|
|  [8]  Display All Rooms          |  [16] Exit Application         |
+===================================================================+
```

1. **Add New Student**:
   - Collects Student ID, Full Name, Age (16-60), Gender, Course & Semester, Phone Number, Total Hostel Fee, and Initial Payment.
   - Enforces unique Student ID.
   - Automatically computes Pending Balance = Total Fee - Amount Paid.
   - Prompts for immediate room allocation (or enqueues to waiting list if all rooms are full).
2. **Display All Students**:
   - Formats registered students in an aligned ASCII table.
3. **Search Student by ID**:
   - Case-insensitive search in $O(N)$ time. Displays a detailed profile card.
4. **Update Student Details**:
   - Allows selective updates to Name, Age, Gender, Course, or Phone.
5. **Delete Student Record**:
   - Requires user confirmation.
   - If student occupied a room, automatically vacates the bed.
   - If a bed becomes vacant and the waiting list has queued students, prompts to allocate the bed to the next waiting student immediately.
   - Deletes from waiting list if present, then frees heap node.
6. **Allocate Hostel Room**:
   - Prevents double-allocation.
   - Shows available rooms with vacant bed counts. Supports auto-allocation or specific room number choice.
7. **Vacate Hostel Room**:
   - Vacate by Student ID or Room Number.
   - Decrements room occupancy and updates room status.
   - Automatically prompts to assign the vacant bed to the front student on the waiting list.
8. **Display All Rooms**:
   - Displays all rooms, capacity, current occupancy, available beds, status (`AVAILABLE`, `PARTIAL`, `FULL`), and names of occupants.
9. **Display Available Rooms**:
   - Filters rooms having at least one vacant bed.
10. **Manage Hostel Fees**:
    - Record payment: Rejects non-positive payments or amounts greater than the pending balance.
    - View financial summary: Total billed, collected, outstanding balance, and collection rate (%).
11. **Display Students with Pending Fees**:
    - Filtered list of students with outstanding dues.
12. **Manage Waiting List**:
    - View FIFO queue, Enqueue unallocated student, Dequeue & allocate, or Remove from queue.
13. **Sort Students by ID**:
    - In-place Linked List **Merge Sort** ($O(N \log N)$).
14. **Sort Students by Name**:
    - In-place Linked List **Merge Sort** ($O(N \log N)$ alphabetical).
15. **Display Hostel Statistics**:
    - Real-time room metrics, student demographics, financial health, and audit action stack view.
16. **Exit Application**:
    - Flushes all changes to disk, deallocates all dynamic memory cleanly, and exits.

---

## 💾 Data Persistence Architecture

The system stores state in a dedicated `data/` directory using human-readable, pipe-delimited text files:

### 1. `data/students.txt`
```text
ID|Name|Age|Gender|Course|Phone|RoomNo|TotalFee|AmountPaid|PendingBalance
STU101|Aarav Sharma|20|Male|B.Tech CSE Sem-4|9876543210|101|60000.00|35000.00|25000.00
STU102|Bhavna Rao|19|Female|B.Tech ECE Sem-2|9876543211|101|65000.00|65000.00|0.00
STU104|Divya Nair|22|Female|M.Tech CSE Sem-2|9876543213|102|55000.00|30000.00|25000.00
```

### 2. `data/rooms.txt`
```text
RoomNo|Capacity|Occupancy|Status
101|2|2|FULL
102|2|1|PARTIAL
103|3|0|AVAILABLE
```

### 3. `data/waiting_list.txt`
```text
StudentID|StudentName|RequestDate
STU105|Eshan Gupta|2026-10-13 22:22:17
```

### Data Integrity Assurance
Upon loading at startup:
- `room_sync_occupancies()` scans the active student linked list and re-derives the exact occupancy count and status for each room.
- This guarantees that corrupt room counts or manual file tampering cannot leave the system in an inconsistent state.

---

## 🖥️ Sample Terminal Output

### Formatted Student Table
```
+----------+----------------------+-----+--------+----------------------+---------------+--------+------------+------------+------------+
| ID       | Name                 | Age | Gender | Course & Sem         | Phone         | Room   | Total Fee  | Paid       | Balance    |
+----------+----------------------+-----+--------+----------------------+---------------+--------+------------+------------+------------+
| STU101   | Aarav Sharma         | 20  | Male   | B.Tech CSE Sem-4     | 9876543210    | R-101  |   60000.00 |   35000.00 |   25000.00 |
| STU102   | Bhavna Rao           | 19  | Female | B.Tech ECE Sem-2     | 9876543211    | R-101  |   65000.00 |   65000.00 |       0.00 |
| STU104   | Divya Nair           | 22  | Female | M.Tech CSE Sem-2     | 9876543213    | R-102  |   55000.00 |   30000.00 |   25000.00 |
+----------+----------------------+-----+--------+----------------------+---------------+--------+------------+------------+------------+
  Total Students: 3
```

### Room Occupancy Table
```
+----------+----------+----------+-----------+------------+------------------------------------------+
| Room No  | Capacity | Occupied | Available | Status     | Current Occupants (Student IDs)          |
+----------+----------+----------+-----------+------------+------------------------------------------+
| 101      | 2        | 2        | 0         | FULL       | STU101, STU102                           |
| 102      | 2        | 1        | 1         | PARTIAL    | STU104                                   |
| 103      | 3        | 0        | 3         | AVAILABLE  | (Vacant)                                 |
| 104      | 3        | 0        | 3         | AVAILABLE  | (Vacant)                                 |
| 201      | 2        | 0        | 2         | AVAILABLE  | (Vacant)                                 |
+----------+----------+----------+-----------+------------+------------------------------------------+
  Total Rooms: 12
```

### Student Profile Card
```
+-------------------------------------------------------------+
|                    STUDENT PROFILE RECORD                   |
+-------------------------------------------------------------+
|  Student ID      : STU101                                   |
|  Full Name       : Aarav Sharma                             |
|  Age             : 20                                       |
|  Gender          : Male                                     |
|  Course & Sem    : B.Tech CSE Sem-4                          |
|  Phone Number    : 9876543210                               |
|  Hostel Room     : Room 101 (Allocated)                     |
+-------------------------------------------------------------+
|  Total Fee       : Rs. 60000.00                             |
|  Amount Paid     : Rs. 35000.00                             |
|  Pending Balance : Rs. 25000.00                             |
+-------------------------------------------------------------+
```

### Administrative Action History (Stack Audit Log)
```
+-----+---------------------+------------------+------------------------------------------------+
| #   | Timestamp           | Action Type      | Description                                    |
+-----+---------------------+------------------+------------------------------------------------+
| 1   | 2026-10-13 22:22:17 | UPDATE STUDENT   | Sorted student records by Name                 |
| 2   | 2026-10-13 22:22:17 | ALLOCATE ROOM    | Allocated Room 101 to waiting student STU105   |
| 3   | 2026-10-13 22:22:17 | VACATE ROOM      | Vacated Room 101 for student STU101            |
| 4   | 2026-10-13 22:22:17 | FEE PAYMENT      | Paid Rs. 15000.00 for student STU101           |
| 5   | 2026-10-13 22:22:17 | ADD STUDENT      | Registered student STU101 (Aarav Sharma)       |
+-----+---------------------+------------------+------------------------------------------------+
  Total Actions in Stack (LIFO): 5
```

---

Team Members

Ankit. Raj 2502220100141,
Ankush kumar singh 2502220100141,
Ankush Raj 2502220100148,
