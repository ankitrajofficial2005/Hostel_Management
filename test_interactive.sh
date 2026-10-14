#!/bin/bash
set -e

echo "=== Cleaning previous test data ==="
rm -rf data

echo "=== Running Interactive Terminal Flow Tests ==="

./hostel_mgmt << 'EOF'
1
STU101
Aarav Sharma
20
Male
B.Tech CSE Sem-4
9876543210
60000.00
20000.00
y
2
101

1
STU102
Bhavna Rao
19
Female
B.Tech ECE Sem-2
9876543211
65000.00
65000.00
y
2
101

1
STU103
Chetan Kumar
21
Male
B.Tech ME Sem-6
9876543212
50000.00
10000.00
y
1

1
STU104
Divya Nair
22
Female
M.Tech AI Sem-1
9876543213
55000.00
30000.00
n

2

3
STU102

3
STU999

4
STU104
4
M.Tech CSE Sem-2

6
STU104
1

8

9

10
2

10
1
STU101
15000.00

11

1
STU105
Eshan Gupta
20
Male
B.Tech IT Sem-4
9876543214
50000.00
25000.00
n

12
2
STU105

12
1

7
1
STU101
y
y

8

13

14

15
y

5
STU103
y

2

16
y
EOF

echo ""
echo "=== Interactive Test Completed Successfully ==="
