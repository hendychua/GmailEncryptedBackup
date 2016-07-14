#include "EncryptionStorageManager.h"

#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/Cipher.h"

#include <iostream>
#include <fstream>

using namespace std;

void EncryptionStorageManager::Persist(const string &userEncryptionKey, const string &filename, const string &payload)
{
    string encryptedPayload = Encrypt(userEncryptionKey, payload);
    string decryptedPayload = Decrypt(userEncryptionKey, encryptedPayload);

    ofstream payloadfile(filename);
    payloadfile << payload;
    payloadfile.close();

    ofstream encryptedPayloadfile(filename + "-enc");
    encryptedPayloadfile << encryptedPayload;
    encryptedPayloadfile.close();

    ofstream decryptedPayloadfile(filename + "-dec");
    decryptedPayloadfile << decryptedPayload;
    decryptedPayloadfile.close();
}

string EncryptionStorageManager::Encrypt(const string &userEncryptionKey, const string &payload)
{
    string encryptedPayload;

    Poco::Crypto::CipherFactory& factory = CipherFactory::defaultFactory();
    // Creates a 256-bit AES cipher
    Cipher* pCipher = factory.createCipher(CipherKey("aes-256", userEncryptionKey));
    encryptedPayload = pCipher->encryptString(payload);

    return encryptedPayload;
}

string EncryptionStorageManager::Decrypt(const string &userEncryptionKey, const string &payload)
{
    string decryptedPayload;

    Poco::Crypto::CipherFactory& factory = CipherFactory::defaultFactory();
    // Creates a 256-bit AES cipher
    Cipher* pCipher = factory.createCipher(CipherKey("aes-256", userEncryptionKey));
    decryptedPayload = pCipher->decryptString(payload);

    return decryptedPayload;
}
