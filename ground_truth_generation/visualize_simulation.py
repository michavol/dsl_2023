import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import matplotlib.patches as patches
import csv
import os
import re

class SimulationPlotter:
    def __init__(self, location_data_file, meta_data_file):
        # Extract simulation name from file name
        self.file_name = location_data_file
        match = re.search(r'\d',self.file_name)
        if match:
            pos = match.start()-1
            self.name = self.file_name[pos:-4]
        else:
            self.name = self.file_name[:-4]

        # Load meta data into dictionary
        with open(meta_data_file, 'r') as data:
            for line in csv.DictReader(data):
                self.meta_data = line

        # Extract important meta data
        self.droplet_radius = float(self.meta_data['droplet_radius'])
        self.larger_droplet = int(self.meta_data['larger_droplet'])
        self.attraction_points = int(self.meta_data['attraction_points'])
        self.repulsion_points = int(self.meta_data['repulsion_points'])
        self.num_droplets = int(self.meta_data["num_droplets"])
        self.screen_width = int(self.meta_data["screen_width"])
        self.screen_height = int(self.meta_data["screen_height"])

        # Store location data in data frame
        self.df = self._read_droplet_data(location_data_file)
        self.droplet_df = self.df[self.df.index.str.startswith('D')]

        if self.larger_droplet:
            self.larger_droplet_df = self.df[self.df.index.str.startswith('L')]
            self.larger_droplet_radius = float(self.meta_data['larger_droplet_radius'])

        if self.attraction_points:
            self.attraction_points_df = self.df[self.df.index.str.startswith('A')]
            self.attraction_radius = float(self.meta_data['attraction_radius'])

        if self.repulsion_points:
            self.repulsion_points_df = self.df[self.df.index.str.startswith('R')]
            self.repulsion_radius = float(self.meta_data['repulsion_radius'])
        
        # Store time points
        self.time_points = self.droplet_df.columns
        self.current_time_point = 0

        # Create figure and buttons
        self.fig, self.ax = plt.subplots(figsize=(self.screen_width/100, self.screen_height/100), dpi=400)

        
        self.ax.axis('equal')
        self.bnext = Button(plt.axes([0.88, 0.013, 0.07, 0.045]), 'Next')
        self.bnext.on_clicked(self.next_time_point)
        self.bprev = Button(plt.axes([0.81, 0.013, 0.07, 0.045]), 'Prev')
        self.bprev.on_clicked(self.next_time_point)
        self.plot_time_point()

    def _read_droplet_data(self, file_name):
        """
        Read droplet data from csv file
        """
        df = pd.read_csv(file_name, index_col=False).T
        df.columns = df.iloc[0].astype(int)
        df = df.drop(df.index[0])
        return df

    def plot_time_point(self):
        self.ax.clear()
        time_point = self.time_points[self.current_time_point]

        # Calculate number of droplets
        current_data = self.droplet_df.iloc[:, self.current_time_point]  
        current_num_droplets = int(len(current_data[current_data!=-1])/2)
            
        # Plot droplets
        for i in range(0, current_num_droplets*2, 2):
            x = current_data[i]
            y = current_data[i+1]
            circle = patches.Circle((x, y), 
                                    radius=self.droplet_radius, 
                                    edgecolor='darkblue', 
                                    facecolor='lightblue',
                                    alpha=0.7,
                                    linewidth=0.1
                                    )
            self.ax.add_patch(circle)
        
        # Plot larger droplets
        if self.larger_droplet:
            x = self.larger_droplet_df.iloc[0,self.current_time_point]
            y = self.larger_droplet_df.iloc[1,self.current_time_point]
            circle = patches.Circle((x, y), 
                                    radius=self.larger_droplet_radius, 
                                    facecolor='black',
                                    edgecolor='black',
                                    alpha=0.4)
            self.ax.add_patch(circle)
        
        # Plot attraction points#
        if self.attraction_points:
            for i in range(0, len(self.attraction_points_df.index), 2):
                x = self.attraction_points_df.iloc[i, self.current_time_point]
                y = self.attraction_points_df.iloc[i+1, self.current_time_point]
                circle = patches.Circle((x, y), 
                                        radius=self.attraction_radius, 
                                        edgecolor='green', 
                                        facecolor='green',
                                        alpha=0.1)
                self.ax.add_patch(circle)
                
            
            
        # Plot repulsion points
        if self.repulsion_points:
            for i in range(0, len(self.repulsion_points_df.index), 2):
                x = self.repulsion_points_df.iloc[i, self.current_time_point]
                y = self.repulsion_points_df.iloc[i+1, self.current_time_point]
                circle = patches.Circle((x, y), self.repulsion_radius, 
                                        edgecolor='red', 
                                        facecolor='red',
                                        alpha=0.1)
                self.ax.add_patch(circle)
        
        self.ax.scatter(0,0,s=0.001) # Necessary for unknown reason 
        
        self.ax.set_title(f'Frame : {time_point} | Num Droplets : {int(current_num_droplets)}')
        plt.draw()

    def next_time_point(self, event):
        self.current_time_point = (self.current_time_point + 1) % len(self.time_points)
        
        self.plot_time_point()

    def prev_time_point(self, event):
        self.current_time_point = self.current_time_point - 1
        if self.current_time_point < 0:
            self.current_time_point = len(self.time_points) - 1
        self.plot_time_point()

    def save_all(self, directory):
        if not os.path.exists(directory):
            os.makedirs(directory)

        directory2 = f'{directory}/plots'

        if not os.path.exists(directory2):
            os.makedirs(directory2)
        
        directory3 = f'{directory2}/{self.name}'

        if not os.path.exists(directory3):
            os.makedirs(directory3)
        
        for i in range(0, len(self.time_points)):
            self.current_time_point = i
            self.plot_time_point()
            self.fig.savefig(f'{directory3}/T{i}.png')

    def show(self):
        plt.show()

if __name__ == '__main__':
    #simulation_plotter = SimulationPlotter('data/location_data_1000_L1_A0_R0.csv', 'data/meta_data_1000_L1_A0_R0.csv')
    #simulation_plotter = SimulationPlotter('data/location_data_1500_L0_A0_R0.csv', 'data/meta_data_1500_L0_A0_R0.csv')
    #simulation_plotter = SimulationPlotter('data/location_data_1000_10_L1_A0_R0.csv','data/meta_data_1000_10_L1_A0_R0.csv')
    #simulation_plotter = SimulationPlotter('data/loc_data_L0_A0_R1_2000_5.csv','data/meta_data_L0_A0_R1_2000_5.csv')
    #simulation_plotter = SimulationPlotter('data/loc_data_L1_A0_R0_3300_4.csv','data/meta_data_L1_A0_R0_3300_4.csv')
    #simulation_plotter = SimulationPlotter('data/loc_data_L1_A0_R0_D1_M1_50_3.csv','data/meta_data_L1_A0_R0_D1_M1_50_3.csv')
    # simulation_plotter = SimulationPlotter('data/loc_data_L1_A0_R0_D1_M1_50_3.csv','data/meta_data_L1_A0_R0_D1_M1_50_3.csv')
    # simulation_plotter1 = SimulationPlotter('data/loc_data_L1_A0_R0_D1_M1_10000_2.csv','data/meta_data_L1_A0_R0_D1_M1_10000_2.csv')
    # simulation_plotter2 = SimulationPlotter('data/loc_data_L1_A0_R1_D1_M1_5000_3.csv','data/meta_data_L1_A0_R1_D1_M1_5000_3.csv')
    # simulation_plotter3 = SimulationPlotter('data/loc_data_L0_A1_R1_D1_M1_5000_3.csv','data/meta_data_L0_A1_R1_D1_M1_5000_3.csv')
    # simulation_plotter4 = SimulationPlotter('data/loc_data_L1_A0_R1_D1_M0_500_10.csv','data/meta_data_L1_A0_R1_D1_M0_500_10.csv')
    # simulation_plotter5 = SimulationPlotter("data/loc_data_L0_A0_R1_D1_M0_1500_5.csv", "data/meta_data_L0_A0_R1_D1_M0_1500_5.csv")
    simulation_plotter6 = SimulationPlotter("data/loc_data_L0_A0_R1_D0_M0_1500_5.csv", "data/meta_data_L0_A0_R1_D0_M0_1500_5.csv")

    
    #simulation_plotter = SimulationPlotter('data/location_data_1500_7_L0_A0_R0.csv','data/meta_data_1500_7_L0_A0_R0.csv')
    #simulation_plotter.show()

    # simulation_plotter.save_all('data')
    # simulation_plotter1.save_all('data')
    # simulation_plotter2.save_all('data')
    # simulation_plotter3.save_all('data')
    # simulation_plotter4.save_all('data')
    # simulation_plotter5.save_all('data')
    simulation_plotter6.save_all('data')