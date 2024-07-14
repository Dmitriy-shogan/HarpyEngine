import trimesh
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# def get_node_transforms(scene):
#     # Инициализация словаря для хранения глобальных трансформаций узлов
#     transforms = {}
#
#     def compute_transform(node_name, parent_transform=np.eye(4)):
#         node = scene.graph.nodes[node_name]
#         # Получение локальной трансформации узла
#         local_transform = scene.graph[node_name].matrix
#         # Вычисление глобальной трансформации как произведение матриц
#         global_transform = parent_transform @ local_transform
#         transforms[node_name] = global_transform
#         # Рекурсивное вычисление трансформаций для детей
#         for child in scene.graph.transforms[node_name].children:
#             compute_transform(child, global_transform)
#
#     # Начало вычисления трансформаций с корневых узлов
#     for root_node in scene.graph.nodes:
#         if scene.graph.transforms[root_node].parent is None:
#             compute_transform(root_node)
#
#     return transforms
#
# def plot_nodes(scene, transforms):
#     fig = plt.figure()
#     ax = fig.add_subplot(111, projection='3d')
#     ax.set_xlabel('X')
#     ax.set_ylabel('Y')
#     ax.set_zlabel('Z')
#
#     for node_name, transform in transforms.items():
#         # Извлечение координат центра узла
#         center = transform[:3, 3]
#         ax.scatter(center[0], center[1], center[2], label=node_name)
#         ax.text(center[0], center[1], center[2], node_name)
#
#     plt.legend()
#     plt.show()

def main():
    # Загрузка модели
    scene = trimesh.load('2CylinderEngine.gltf')
    scene.show();

    # # Получение глобальных трансформаций узлов
    # transforms = get_node_transforms(scene)
    #
    # # Визуализация центров узлов
    # plot_nodes(scene, transforms)

if __name__ == "__main__":
    main()
