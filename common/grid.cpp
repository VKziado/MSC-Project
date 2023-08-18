#include "grid.h"

std::unique_ptr<TechniqueGrid> Grid::_technique;
Grid::Grid(Node* parent) : Component(parent) {
    if (!_technique) {
        _technique = std::make_unique<TechniqueGrid>();
    }
}
void Grid::draw() {
    _technique->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}