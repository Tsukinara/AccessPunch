AccessPunch
===========

This program is designed to be used with a raspberry pi and a magnetic 3-track keycard reader.
The program itself constantly waits for a card to be scanned from the reader, and then parses the data from the card into a human-readable form. 
If the card scanned is a student ID, it will check to see if the ID is in the whitelist. 
Given that it is, the program will then execute commands to open the door from the inside. 

In all other situations, nothing will happen, except for the failure audio being played.

If a credit/debit card is scanned, the program will make fun of you for being retarded.
