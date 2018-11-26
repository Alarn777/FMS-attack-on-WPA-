# FMS-attack-on-WPA-
To run the program you need a traffic.rcf with IV3 encoding file 

    How does this work:
    Assuming we have enough messages interrupted.
    We know that almost all messages start with 0xAA
    And that the message is combined: IV||(P(IV||key))

    We use special "weak" messages of 5 bytes of structure:
    bit_we_hack+3 : 0xFF : any_muber
    As KSA takes as input key[8] we assume that first bit_we_hack+3 bits of the key are the one we got form "weak" key.
    We gather all this messages and do only first bit_we_hack+3 steps of KSA algorithm on them
    when we reach step bit_we_hack+3 we now have the candidate 1byte bit of the real key
    But we have alot of messages and a lot of candidate keys. So we make an array of size 256 and count how
    may times each candidate key was produced. The one candidate key that has the most appearances will be our first byte in the real key
    as we now know the firs byte of the key we can derive next one using the same tactic, but now we have to initialize key[8] with
    first 3 bytes of the "weak key" + the byte we just found out.


