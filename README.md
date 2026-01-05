# hackathon-project---emergency-complaint-submission
Emergency Complaint Submission  
Project Overview

Live project page:https://sites.google.com/view/emergency-report-submission/home

This project is a C-based command line system that collects and processes emergency complaints submitted by users through Google Forms.

The responses are stored in Google Sheets, fetched live by the program, and analyzed to prioritize emergencies based on severity and generate a clear report for authorities.

Problem it tackles:
Emergency complaints are often:
Unorganized
Delayed
Hard to prioritize
This makes response time slow during critical situations.

Our Solution:
Users submit complaints using Google Forms
Data is automatically stored in Google Sheets
The C program:
Downloads the sheet as CSV
Sorts complaints by severity
Groups them by location
Generates a summarized emergency report

Technologies used:
Language: C
Google Forms – Data collection
Google Sheets – Live database
libcurl – Fetching live CSV data
GCC / WSL (Ubuntu) – Development environment

Key features
Live data fetching from Google Sheets
Severity-based prioritization
Location-wise complaint grouping
Simple CLI menu
Report generation in text format

How to compile:
gcc code.c -o code -lcurl

How to run:
./code

Future Improvements:
Web Dashboard Interface: 
Add a simple web dashboard to visualize complaints using charts and tables, making it easier for authorities to monitor emergencies in real time.

Automatic Data Refresh:
Schedule periodic data fetching from Google Sheets so the system updates automatically without manual refresh.

SMS / Email Alerts:
Send instant alerts to authorities when a high-severity complaint is submitted.

Geo-Location Mapping: 
Integrate Google Maps to display complaint locations visually, helping responders reach the exact observation faster.

Role-Based Access:
Different access levels for administrators, responders, and viewers to improve security and accountability.

Mobile Application Support:
Extend the system to a mobile app for faster complaint submission and tracking.

Machine Learning Prioritization:
Use basic ML models to automatically assess complaint severity based on keywords and past data.

Offline Data Backup:
Store local backups of complaint data to ensure availability even if internet access fails.

