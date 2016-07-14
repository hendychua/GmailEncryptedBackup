# GmailEncryptedBackup
Having fun with Gmail APIs. This CLI program retrieves your email (filtered by start and end date) and stores 3 versions (unencrypted, encrypted, decrypted) of each email in your folder. The encryption use is Aes-256.

## Dependencies
1. Boost
2. Casablanca (Microsoft's CPP Rest SDK)
3. Poco (for its Crypto library)
4. openssl has to be installed for Casablanca and Poco::Crypto to work.

## Compilation
This CLI program uses Rest APIs by Google, hence it requires a Client ID and Client Secret provided by Google when you register your application. It is unsafe to store Client ID and Client Secret in source code. Hence you need to provide both of them when compiling. The compilation command is:

<code>
g++ -o <programName> -DCLIENT_ID='"<clientId>"' -DCLIENT_SECRET='"<clientSecret>"' main.cpp GoogleAuthorizationClient.cpp GmailMessageClient.cpp EncryptionStorageManager.cpp -std=c++11 -I<path to casablanca>/Casablanca/include -lcrypto -lssl -lboost_system -lboost_filesystem -L<path to casablanca>/Casablanca/lib -lcpprest -lboost_thread -I<path to crypto>/Cryptopp/include -L<path to Crytopp>/Cryptopp/lib -lcryptopp -I<path to Poco>/Poco/include -L<path to Poco>/Poco/lib -lPocoCrypto
</code>

Obviously, the compilation command assumes that you installed Boost with your package manager and it is in your /usr/lib folder. If your boost is in another folder, you have to include the lib folder and include folder inside the compilation command.

Feel free to use any build system you like to make the compilation easier. In fact I am trying to use cmake and I welcome any pull requests for a CMakeLists.txt file.

## Usage
1. The program expects 3 inputs to start: <email address> <start date of filter> <end date>
2. start and end date should be in yyyy/mm/dd format
3. The program will prompt you for a secret key use for encryption. You have to enter twice to confirm. Your secret key will not be printed on the screen when you are typing it.
4. Follow the instructions in the console to authorize the app to retrieve emails for you. You have to copy paste URL into your browser and allow the app access and then copy the redirected URL back into the console.
5. Emails encrypted backups will be stored in a folder called backups in your working directory.


## Notes
Feel free to contact me if you find any bugs.
