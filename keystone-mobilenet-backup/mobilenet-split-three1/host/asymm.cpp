#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <iostream>
#include <string>
#include <memory>
#include <cstring>

void handleOpenSSLErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

std::pair<std::string, std::string> generateRSAKeyPair() {
    // Create an EVP keypair
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        handleOpenSSLErrors();
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        handleOpenSSLErrors();
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        handleOpenSSLErrors();
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        handleOpenSSLErrors();
    }

    EVP_PKEY_CTX_free(ctx);

    // Write the keys to memory
    BIO *privateKeyBio = BIO_new(BIO_s_mem());
    BIO *publicKeyBio = BIO_new(BIO_s_mem());

    if (!PEM_write_bio_PrivateKey(privateKeyBio, pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        handleOpenSSLErrors();
    }

    if (!PEM_write_bio_PUBKEY(publicKeyBio, pkey)) {
        handleOpenSSLErrors();
    }

    char *privateKeyData;
    long privateKeyLen = BIO_get_mem_data(privateKeyBio, &privateKeyData);
    std::string privateKey(privateKeyData, privateKeyLen);

    char *publicKeyData;
    long publicKeyLen = BIO_get_mem_data(publicKeyBio, &publicKeyData);
    std::string publicKey(publicKeyData, publicKeyLen);

    BIO_free(privateKeyBio);
    BIO_free(publicKeyBio);
    EVP_PKEY_free(pkey);

    return {publicKey, privateKey};
}

std::string encryptWithPublicKey(const std::string &message, const std::string &publicKey) {
    BIO *bio = BIO_new_mem_buf(publicKey.data(), -1);
    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (!pkey) {
        handleOpenSSLErrors();
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0) {
        handleOpenSSLErrors();
    }

    size_t outlen = 0;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, reinterpret_cast<const unsigned char *>(message.c_str()),
                         message.size()) <= 0) {
        handleOpenSSLErrors();
    }

    std::string encrypted(outlen, '\0');
    if (EVP_PKEY_encrypt(ctx, reinterpret_cast<unsigned char *>(&encrypted[0]), &outlen,
                         reinterpret_cast<const unsigned char *>(message.c_str()), message.size()) <= 0) {
        handleOpenSSLErrors();
    }

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return encrypted;
}

std::string decryptWithPrivateKey(const std::string &encrypted, const std::string &privateKey) {
    BIO *bio = BIO_new_mem_buf(privateKey.data(), -1);
    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    if (!pkey) {
        handleOpenSSLErrors();
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx || EVP_PKEY_decrypt_init(ctx) <= 0) {
        handleOpenSSLErrors();
    }

    size_t outlen = 0;
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, reinterpret_cast<const unsigned char *>(encrypted.c_str()),
                         encrypted.size()) <= 0) {
        handleOpenSSLErrors();
    }

    std::string decrypted(outlen, '\0');
    if (EVP_PKEY_decrypt(ctx, reinterpret_cast<unsigned char *>(&decrypted[0]), &outlen,
                         reinterpret_cast<const unsigned char *>(encrypted.c_str()), encrypted.size()) <= 0) {
        handleOpenSSLErrors();
    }

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return decrypted;
}

int main() {
    auto [publicKey, privateKey] = generateRSAKeyPair();
    std::cout << "Public Key:\n" << publicKey << "\n";
    std::cout << "Private Key:\n" << privateKey << "\n";

    std::string message = "Hello, OpenSSL 3.0!";
    std::cout << "Original Message: " << message << "\n";

    std::string encrypted = encryptWithPublicKey(message, publicKey);
    std::cout << "Encrypted Message: " << encrypted << "\n";

    std::string decrypted = decryptWithPrivateKey(encrypted, privateKey);
    std::cout << "Decrypted Message: " << decrypted << "\n";

    return 0;
}
