import numpy as np 
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter
import re
import matplotlib.patches as patches

def read_position(s):
    pattern = r'\s*(\d+)\s*\(\s*(-?[0-9.eE+-]+)\s*,\s*(-?[0-9.eE+-]+)\s*\)\s*\(\s*(-?\d+)\s*,\s*(-?\d+)\s*\)\s*'
    match = re.match(pattern, s)
    
    if match:
        index = int(match.group(1))
        x = float(match.group(2))
        y = float(match.group(3))
        u = float(match.group(4))
        v = float(match.group(5))
        
        return index, x, y, u, v 
    else:
        print(f"line = {s}")
        raise ValueError("String format is incorrect")



def plot_points(positions, time, index, point_size):
    """
    Affiche les points pour un instant donné indiqué par son indice dans la liste.
    
    Parameters:
    positions (list of np.array): Liste des coordonnées des points.
    time (list of float): Liste des instants correspondants.
    index (int): Indice de l'instant à afficher.
    """
    if index < 0 or index >= len(positions):
        raise ValueError("Index out of bounds")
    
    points = positions[index]
    timestamp = time[index]
    
    N = len(positions[0])
    # point_size = 1/N*100
    
    plt.figure()
    point_size *= 50
    plt.scatter(points[:, 0], points[:, 1], c='blue', marker='d', s=point_size**2)
    plt.xlim(-1, 1)
    plt.ylim(-1, 1)
    borne = np.sqrt(2)/2
    plt.plot([-borne, 0, borne, 0, -borne], [0, borne, 0, -borne, 0], '-k')
    # plt.scatter(points[:, 0], points[:, 1], c='blue', marker='d', s=epsilon**2 * 1000)
    
    plt.title(f'Time: {timestamp:.2f}')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.show()
    # plt.show(block=False)

# def animate_points(positions, time, output_file='animation.mp4', point_size=0.001):
#     """
#     Crée une animation en affichant les points et le time code correspondant sur l'image
#     et enregistre l'animation dans un fichier MP4.
    
#     Parameters:
#     positions (list of np.array): Liste des coordonnées des points.
#     time (list of float): Liste des instants correspondants.
#     output_file (str): Nom du fichier de sortie pour l'animation MP4.
#     """
#     fig, ax = plt.subplots()
#     # scatter = ax.scatter([], [])
#     point_size *= 50
#     scatter = ax.scatter([], [], marker='d', s=point_size**2 *10000)  # Losanges de taille epsilon ? 

#     title = ax.set_title('')
    
#     def init():
#         ax.set_xlim(-0.8, 0.8)
#         ax.set_ylim(-0.8, 0.8)
#         borne = np.sqrt(2)/2
#         ax.plot([-borne, 0, borne, 0, -borne], [0, borne, 0, -borne, 0], '-k')
#         return scatter, title

#     def update(frame):
#         points = positions[frame]
#         num_points = len(points)
#         colors = ['black'] * num_points
        
#         # If two particles are colliding, they're drawn in red  
#         for i in range(num_points):
#             for j in range(i + 1, num_points):
#                 if np.linalg.norm(points[i] - points[j]) < eps:
#                     colors[i] = 'red'
#                     colors[j] = 'red'
        
#         scatter.set_offsets(points)
#         scatter.set_color(colors)
#         title.set_text(f'Time: {time[frame]:.2f}')
#         return scatter, title
    
#     ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=True, interval=200)
    
#     # Saving 
#     writer = FFMpegWriter(fps=10, metadata=dict(artist='Me'), bitrate=1800)
#     ani.save(output_file, writer=writer)
    
#     plt.close(fig)

def animate_points(positions, time, output_file='animation.mp4', point_size=0.001):
    """
    Crée une animation en affichant les points et le time code correspondant sur l'image
    et enregistre l'animation dans un fichier MP4.
    
    Parameters:
    positions (list of np.array): Liste des coordonnées des points.
    time (list of float): Liste des instants correspondants.
    output_file (str): Nom du fichier de sortie pour l'animation MP4.
    point_size (float): Taille du côté du losange.
    """
    fig, ax = plt.subplots()
    title = ax.set_title('')

    def init():
        ax.set_xlim(-0.8, 0.8)
        ax.set_ylim(-0.8, 0.8)
        borne = np.sqrt(2)/2
        ax.plot([-borne, 0, borne, 0, -borne], [0, borne, 0, -borne, 0], '-k')
        return []

    def create_diamond(x, y, size):
        half_size = size / 2
        diamond = np.array([[x - half_size, y], [x, y + half_size], 
                            [x + half_size, y], [x, y - half_size]])
        return patches.Polygon(diamond, closed=True)

    def update(frame):
        ax.clear()
        init()  # Reset the plot to initial settings

        points = positions[frame]
        num_points = len(points)
        patches_list = []

        for i in range(num_points):
            x, y = points[i]
            losange = create_diamond(x, y, point_size)
            losange.set_color('black')
            patches_list.append(losange)
            ax.add_patch(losange)
        
        # Check for collisions
        for i in range(num_points):
            for j in range(i + 1, num_points):
                if np.linalg.norm(points[i] - points[j]) < point_size:
                    patches_list[i].set_color('red')
                    patches_list[j].set_color('red')

        title.set_text(f'Time: {time[frame]:.2f}')
        return patches_list + [title]

    ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=True, interval=200)
    
    # Saving 
    writer = FFMpegWriter(fps=10, metadata=dict(artist='Me'), bitrate=1800)
    ani.save(output_file, writer=writer)
    
    plt.close(fig)


f = open("../output/summary.ushi", "r")

# reading header 
line = f.readline()
while line[0] == '-':
    line = f.readline()

numbers = re.findall(r'\d+', line)
print(numbers)
numbers = [float(num) for num in re.findall(r'-?\d+(?:\.\d+)?(?:e-?\d+)?', line)]
print(numbers)
N = int(numbers[0])
eps = float(numbers[1])
# epsilon = 1 / N
endtime = int(numbers[2])

print(f"reading uchi file, N = {N} particles, eps = {eps}, endtime = {endtime}")




# reading times and positions 
time = []
positions = []
while len(line) != 0:
    line = ''
    while len(line) < 2:
        line = f.readline()
        if len(line) == 0:
            break
    if len(line) == 0:
        break
    t = float(re.findall(r'\d+(?:\.\d+)?', line)[0])
    pos = np.zeros((N, 2))
    # print(f"I read t = {t}")
    for iPoint in np.arange(N):
        line = f.readline()
        # print(line)
        index, x, y, u, v = read_position(line)
        pos[iPoint, :] = np.array([x, y])
    time.append(t)
    positions.append(pos)




n_time = len(time)
print(f"n_time = {n_time}")
#n_time = 100
#positions = positions[0:100]

point_size = eps 
print(f"epsilon attendu : {eps}")
animate_points(positions, time, point_size=eps)
# plot_points(positions, time, n_time-1, eps)
### plot_points(positions, time, n_time-1, point_size*50)
f.close() 

