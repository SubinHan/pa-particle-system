#pragma once

class UiLink
{
public:
    UiLink() = default;
    UiLink(int id, int from, int to);

    int getId() const;
    int getFromId() const;
    int getToId() const;
    int getOther(int id) const;

private:
    int _id;
    int _from;
    int _to;
};