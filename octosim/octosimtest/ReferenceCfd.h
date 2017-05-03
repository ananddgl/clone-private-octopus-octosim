#pragma once
class ReferenceCfd
{
public:
    ReferenceCfd();
    ~ReferenceCfd();

    static const double * Proba();
    static const unsigned long long * Arrival();
    static const unsigned long long * Authoritative();
    static const unsigned int * QueryLength();
    static const unsigned int * ResponseLength();
};

