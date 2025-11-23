#pragma once

class EditorHierarchy {

public:
    EditorHierarchy(std::shared_ptr<Node> );
    ~EditorHierarchy();

    virtual void Draw();

};