import PySimpleGUI as sg
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# VARS CONSTS:

# New figure and plot variables so we can manipulate them
line1 = False
line2 = False

_VARS = {'window': False,
         'fig_agg': False,
         'pltFig': False}

dataSize = 1000  # For synthetic data

# Helper Functions


def draw_figure(canvas, figure):
    figure_canvas_agg = FigureCanvasTkAgg(figure, canvas)
    figure_canvas_agg.draw()
    figure_canvas_agg.get_tk_widget().pack(side='top', fill='both', expand=1)
    return figure_canvas_agg


# \\  -------- PYSIMPLEGUI -------- //

AppFont = 'Any 16'
sg.theme('DarkTeal12')

layout = [[sg.Canvas(key='figCanvas')],
          [sg.Button('Update', font=AppFont), sg.Button('Exit', font=AppFont)]]
_VARS['window'] = sg.Window('Such Window',
                            layout,
                            finalize=True,
                            resizable=True,
                            location=(100, 100),
                            element_justification="right")

# \\  -------- PYSIMPLEGUI -------- //



# \\  -------- PYPLOT -------- //


def makeSynthData():
    xData = np.random.randint(100, size=dataSize)
    yData = np.linspace(0, dataSize, num=dataSize, dtype=int)
    return (xData, yData)


def drawChart():
    _VARS['pltFig'], axs = plt.subplots(2)
    dataXY = makeSynthData()
    line1, = axs[0].plot(dataXY[0], dataXY[1], '.k')
    line2, = axs[1].plot(dataXY[0], dataXY[1], '.k')
    print(axs)
    _VARS['fig_agg'] = draw_figure(
        _VARS['window']['figCanvas'].TKCanvas, _VARS['pltFig'])


# Recreate Synthetic data, clear existing figre and redraw plot.

def updateChart(x, y):
    _VARS['fig_agg'].get_tk_widget().forget()
    dataXY = makeSynthData()
    # plt.cla()
    plt.clf()
    _VARS['pltFig'], axs = plt.subplots(2)
    line1, = axs[0].plot(x, y, '.k')
    line2, = axs[1].plot(x, y, '.k')
    _VARS['fig_agg'] = draw_figure(
        _VARS['window']['figCanvas'].TKCanvas, _VARS['pltFig'])

# \\  -------- PYPLOT -------- //


drawChart()
x,y = makeSynthData()
# MAIN LOOP
while True:
    event, values = _VARS['window'].read(timeout=200)
    if event == sg.WIN_CLOSED or event == 'Exit':
        break
    # New Button (check the layout) and event catcher for the plot update
    
    if event == 'Update':
        updateChart(x,y)
_VARS['window'].close()