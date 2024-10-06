#ifndef COMPONENTS_H
#define COMPONENTS_H

/**
 * Maximum number of components. Increase if needed
 */
#define gComponentsMax 256

/**
 * Convert component type to component id
 */
#define _componentId(componentName) __##componentName##_id

/**
 * Get component size by component type
 */
#define _componentSize(componentName) __##componentName##_size

/**
 * Define component structure
 */
#define _component(componentName, ...) typedef struct __VA_ARGS__ componentName; \
    size_t _componentId(componentName) = -1; \
    size_t _componentSize(componentName) = sizeof(componentName);

/**
 * Define tag component
 */
#define _tagComponent(componentName) typedef struct {} componentName; \
    size_t _componentId(componentName) = -1; \
    size_t _componentSize(componentName) = 0;

/**
 * Register component in the world
 */
#define _addComponentToWorld(componentName) { \
    gComponentData cd = {.size = _componentSize(componentName)}; \
    __##componentName##_id = gWorldRegComponent(__cw, cd); \
}

#endif //COMPONENTS_H
