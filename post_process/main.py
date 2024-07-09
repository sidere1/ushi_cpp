import numpy as np 
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter
import re

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
    plt.scatter(points[:, 0], points[:, 1], c='blue', marker='d', s=point_size**2)
    # plt.scatter(points[:, 0], points[:, 1], c='blue', marker='d', s=epsilon**2 * 1000)
    
    plt.title(f'Time: {timestamp:.2f}')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.grid(True)
    plt.show()
    # plt.show(block=False)

def animate_points(positions, time, output_file='animation.mp4'):
    """
    Crée une animation en affichant les points et le time code correspondant sur l'image
    et enregistre l'animation dans un fichier MP4.
    
    Parameters:
    positions (list of np.array): Liste des coordonnées des points.
    time (list of float): Liste des instants correspondants.
    output_file (str): Nom du fichier de sortie pour l'animation MP4.
    """
    fig, ax = plt.subplots()
    # scatter = ax.scatter([], [])
    scatter = ax.scatter([], [], marker='d', s=epsilon**2 * 1000)  # Losanges de taille epsilon

    title = ax.set_title('')
    
    def init():
        ax.set_xlim(np.min([pos[:, 0].min() for pos in positions]), np.max([pos[:, 0].max() for pos in positions]))
        ax.set_ylim(np.min([pos[:, 1].min() for pos in positions]), np.max([pos[:, 1].max() for pos in positions]))
        return scatter, title
    
    def update(frame):
        points = positions[frame]
        scatter.set_offsets(points)
        title.set_text(f'Time: {time[frame]:.2f}')
        return scatter, title
    
    ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=True, interval=200)
    
    # Enregistrer l'animation en fichier MP4
    writer = FFMpegWriter(fps=10, metadata=dict(artist='Me'), bitrate=1800)
    ani.save(output_file, writer=writer)
    
    plt.close(fig)
    
    
# Example usage:
# positions = [np.random.rand(10, 2) for _ in range(100)]  # Replace with your data
# time = np.linspace(0, 10, 100)  # Replace with your data
# plot_points(positions, time, 0)  # Plot points at the first instant
# animate_points(positions, time)  # Create an animation






f = open("../summary.ushi", "r")

# reading header 
line = f.readline()
while line[0] == '-':
    line = f.readline()

numbers = re.findall(r'\d+', line)
N = int(numbers[0])
eps = int(numbers[1])
epsilon = 1 / N
endtime = int(numbers[2])

print(f"reading ushi file, N = {N} particles, eps = {eps}, endtime = {endtime}")




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
        index, x, y, u, v = read_position(line)
        pos[iPoint, :] = np.array([x, y])
    time.append(t)
    positions.append(pos)




n_time = len(time)
n_time = 100
positions = positions[0:100]

point_size = epsilon 
animate_points(positions, time)
plot_points(positions, time, 99, point_size*50)
f.close() 

