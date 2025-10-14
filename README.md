# C++ implementation of the Diffie-Hellman key exchange algorithm

## Quick overview

The Diffie-Hellman key exchange (`DHKE`) algorithm is used for establishing a secure method of communication over a public/unsecure network.

Let's examine a (very) simplified explanation of how it works, using "Alice" and "Bob" as our two communicating individuals:

1. It starts with a key exchange process, where Alice and Bob agree on a shared set of numbers (parameters). **At this point, all communication is unencrypted and can be viewed by anyone**. 
2. Both Alice and Bob also have their own super secret keys that they haven't told anyone about, not even each other.
3. Alice and Bob both take the shared parameters and combine it with their respective secret keys, giving them a new key.
4. Then, Alice and Bob swap their newly generated keys with each other.
5. Finally, Alice takes the key Bob just gave her and combines it with her secret key again, giving her the final key. Bob does the exact same with the generated key Alice just gave him.
6. **Even though neither of them knew the other's secret key, both Alice and Bob now have the exact same final key**.
7. This "final key" can then be used in a standard encryption method (e.g. AES) to now send encrypted data securely.

### But what if someone was listening to their initial key exchanges?
The benefit of `DHKE` is that it doesn't matter if someone was listening to the initial key exchange. The 'eavesdropper' could even attempt to create their own "final key", but it won't match the final key that both Alice and Bob generated.

The key aspect is that **the eavesdropper doesn't know either of Alice's or Bob's secret keys**, because they were never shared. Yet without either of those secret keys, no one can generate the exact same "final key" that Alice and Bob are able to generate.

The security comes from the same idea behind most modern encryption algorithms: it involves a **"very hard problem"**. Even if you had the data from the initial key exchange, plus the encrypted communication data, trying to reverse the process used to generate the "final key" is so computationally expensive that it can't reasonably be done by any modern classical computer.