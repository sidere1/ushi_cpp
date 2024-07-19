import numpy as np 
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter
import re
import matplotlib.patches as patches

def read_position(s):
    """
    Parse a string to extract the coordinates and other information of a point.

    Parameters:
    s (str): The string containing the point information.

    Returns:
    tuple: A tuple containing the index, x and y coordinates, and u and v values.

    Raises:
    ValueError: If the string format is incorrect.
    """
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
    Plot the points for a given instant specified by its index in the list.

    Parameters:
    positions (list of np.array): List of point coordinates.
    time (list of float): List of corresponding instants.
    index (int): Index of the instant to display.
    point_size (float): Size of the side of the diamond.

    Raises:
    ValueError: If the index is out of bounds.
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


def animate_points(positions, time, output_file='animation.mp4', point_size=0.001, impactLocations=[]):
    """
    Create an animation displaying the points and the corresponding time code on the image
    and save the animation to an MP4 file.

    Parameters:
    positions (list of np.array): List of point coordinates.
    time (list of float): List of corresponding instants.
    output_file (str): Name of the output file for the MP4 animation.
    point_size (float): Size of the side of the diamond.
    impactLocations (np.array): List of impact locations for each frame.
    """
    fig, ax = plt.subplots()
    title = ax.set_title('')
    drawImpacts = len(impactLocations) == len(time)

    def init():
        ax.set_xlim(-0.8, 0.8)
        ax.set_ylim(-0.8, 0.8)
        ax.set_aspect('equal', adjustable='box')
        borne = np.sqrt(2)/2
        ax.plot([-borne, 0, borne, 0, -borne], [0, borne, 0, -borne, 0], '-k')
        ax.set_aspect('equal')
        return []

    def create_diamond(x, y, size):
        """
        Create a diamond-shaped patch.

        Parameters:
        x (float): X coordinate of the center of the diamond.
        y (float): Y coordinate of the center of the diamond.
        size (float): Size of the side of the diamond.

        Returns:
        patches.Polygon: A polygon representing the diamond.
        """
        half_size = size / 2
        diamond = np.array([[x - half_size, y], [x, y + half_size], 
                            [x + half_size, y], [x, y - half_size]])
        return patches.Polygon(diamond, closed=True)
    
    def create_circle(x, y, size):
        """
        Create a circle-shaped patch.

        Parameters:
        x (float): X coordinate of the center of the circle.
        y (float): Y coordinate of the center of the circle.
        size (float): Diameter of the circle.

        Returns:
        patches.Circle: A circle patch.
        """
        radius = size / 2
        circle = patches.Circle((x, y), radius)
        return circle

    def update(frame):
        ax.clear()
        init()

        points = positions[frame]
        impactPoint = impactLocations[frame, :]
        num_points = len(points)
        patches_list = []

        if drawImpacts:
            # print(impactPoint)
            # impactLoc = create_circle(impactPoint[0, 0], impactPoint[0,1], point_size*4)
            impactLoc = create_circle(impactPoint[0], impactPoint[1], point_size*2)
            impactLoc.set_color('red')
            ax.add_patch(impactLoc)

        for i in range(num_points):
            x, y = points[i]
            losange = create_diamond(x, y, point_size)
            losange.set_color('black')
            patches_list.append(losange)
            ax.add_patch(losange)
        
        ax.add_patch(losange)
        title.set_text(f'Time: {time[frame]:.2f}')
        return patches_list + [title]

    ani = FuncAnimation(fig, update, frames=len(positions), init_func=init, blit=True, interval=200)
    
    # Saving 
    writer = FFMpegWriter(fps=10, metadata=dict(artist='Me'), bitrate=1800)
    ani.save(output_file, writer=writer)
    
    plt.close(fig)


f = open("../output/summary.uchi", "r")

# reading header 
line = f.readline()
while line[0] == '-':
    line = f.readline()

numbers = [float(num) for num in re.findall(r'-?\d+(?:\.\d+)?(?:e-?\d+)?', line)]
print(numbers)
N = int(numbers[0])
eps = float(numbers[1])
# epsilon = 1 / N
endtime = float(numbers[2])

print(f"reading uchi file, N = {N} particles, eps = {eps}, endtime = {endtime}")




# reading times and positions 
time = []
positions = []
impactLoc = np.zeros([1,2])
impactLocations = np.zeros([0,2])
while len(line) != 0:
    line = ''
    while len(line) < 2:
        line = f.readline()
        if len(line) == 0:
            break
    if len(line) == 0:
        break
    t = float(re.findall(r'\d+(?:\.\d+)?', line)[0])
    line = f.readline()
    impactLoc[0,0] = float(re.findall(r'-?\d+(?:\.\d+)?(?:e-?\d+)?', line)[0])
    line = f.readline()
    impactLoc[0,1] = float(re.findall(r'-?\d+(?:\.\d+)?(?:e-?\d+)?', line)[0])
    pos = np.zeros((N, 2))
    for iPoint in np.arange(N):
        line = f.readline()
        index, x, y, u, v = read_position(line)
        pos[iPoint, :] = np.array([x, y])
    time.append(t)
    impactLocations = np.append(impactLocations, impactLoc, axis=0)
    positions.append(pos)




n_time = len(time)
print(f"n_time = {n_time} ; eps = {eps}")

point_size = eps 
animate_points(positions, time, point_size=eps, impactLocations=impactLocations)
# plot_points(positions, time, n_time-1, eps)
### plot_points(positions, time, n_time-1, point_size*50)
f.close()