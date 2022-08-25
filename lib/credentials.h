// const char* SSID = "WiFi-Plessers";
// const char* PWD = "JpHh1973";

// const char* SSID = "CasaKota";
// const char* PWD = "arrosticini";

#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

struct Authentication {
    const char* SSID;
    const char* PWD;
};

class Auth{
    public:
    Authentication getPlessers()
    {
        return Plessers;
    }

    Authentication getLoiacono()
    {
        return Loiacono;
    }

    private:
        Authentication Plessers = {"WiFi-Plessers", "JpHh1973"};
        Authentication Loiacono = {"CasaKota", "arrosticini"};
};
#endif //AUTHENTICATION_H