#ifndef GROUPCONTAINER_H
#define GROUPCONTAINER_H

#include <vector>
#include "models/group.h"

class GroupContainer
{
public:
    static GroupContainer& getInstance();

    void addGroup(Group group);
    void addGroupFromDb(Group group);
    Group getGroup(int index);
    void setName(int id, QString name);
    void setColor(int id, QString color);
    void removeGroup(int id);

    size_t groupCount();

private:
    GroupContainer() = default;

    std::vector<Group> _group_vec;
    int findGroup(int id, QString method);
};
#define group_container GroupContainer::getInstance()

#endif // GROUPCONTAINER_H
