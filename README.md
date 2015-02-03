Crypto Challenge "B":

Summary: Two files are available to the solvers: "flag.encrypted", which contains the flag encrypted by the second file "encryptor.exe". Encryptor.exe is a Windows executable.  I've included the source file encryptor.c in the top directory.  It uses the OpenSSL library but other than that is straightforward C; if the students will be using a different platform this would need to be recompiled.  Sorry, I'm not good with Makefiles.  

Challenge files:

	index.html -- The challenge entry point.
	/files -- this should be a readable sub-folder.
      /files/flag.encrypted -- pointed to by index.html
      /files/encryptor.exe -- pointed to by index.html

Solution walkthrough:

There is a hard way and an easy way to do this challenge.  The hard way is to decompile the executable, understand how it uses the OpenSSL library to encrypt, recover the keys used, and make a program that uses those keys to decrypt.  The students will also have to understand that the IV is included in the ciphertext and parse it back out in order to decrypt correctly.  

The easy way to do this challenge is that after playing with the executable for a while, the students will discover that the first 8 bytes of the output are always the first 8 bytes of the first input file, and the remaining bytes of the output are determined by the first 8 bytes of the first input and by the second input.  A simple script can try all 2^32 potential flags to try to produce a matching output.  

The issue of padding adds some uncertainty, but if students pad a flag string to 16 bytes, the output size would not be correct. I hope this leads to them being confident that the input is not padded.

	
"IV" is a file to use for the IV when encrypting which matches the IV used for flag.encrypted. "flag" is the correct flag.

You can verify that running encryptor.exe on "IV flag foo" produces a file foo that matches flag.encrypted.  Once the brute force script is written properly it should take about 30 minutes to find the right flag.

