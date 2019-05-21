import dash
from dash.dependencies import Output, Input, State
import dash_core_components as dcc
import dash_html_components as html
from collections import deque
import serial
import time

app = dash.Dash()
app.css.append_css({'external_url': 'https://codepen.io/amyoshino/pen/jzXypZ.css'})  # noqa: E501
colors = {
    'background': '#111111',
    'text':'#7FDBFF'
}

app.layout = html.Div(style={'backgroundColor':colors['background']},children=[
    html.H1(
        children='Jazz Project Dashboard',
        style={
            'textAlign': 'center',
            'color':colors['text']
        }
    ),

    html.H3(
        children='some information about the Jazz project and what problem it is trying to solve.',
        style={
            'textAlign': 'center',
            'color':colors['text']
        }
    ),
    html.Div([
        html.Div([
            html.Div(
                'Initial Frequency (Hz): ',
                style={'color': colors['text'],'textAlign':'right'},
                className = "five columns"
            ),
            html.Div(
                dcc.Input(
                    id = 'freq',
                    placeholder='Initial Frequency (Hz)',
                    type='number',
                    #value='2000',
                    debounce='True'
                ),
                className="five columns"
            )
        ],className="row"),
        html.Div([
            html.Div(
                'Measured Frequency (Hz): ',
                style={'color': colors['text'],'textAlign':'right'},
                className = "five columns"
            ),
            html.Div(
                dcc.Input(
                    id='measuredFreq',
                    placeholder='Measured Frequency (Hz)',
                    type='number',
                    #value='23',
                    disabled='True'
                ),
                className="five columns"
            ),
        ],className="row"),
        html.Div([
            html.Div(
                'Temperature (C): ',
                style={'color': colors['text'],'textAlign':'right'},
                className = "five columns"
            ),
            html.Div(
                dcc.Input(
                    id='temp',
                    placeholder='Temperature (C)',
                    type='number',
                    #value='23',
                    #debounce='True',
                    disabled = 'True'
                ),
                className="five columns"
            )
        ],className="row"),
        html.Div([
            html.Div(
                'Results: ',
                style={'color': colors['text'],'textAlign':'right'},
                className = "five columns"
            ),
            html.Div(
                dcc.Textarea(
                    id='status',
                    placeholder='Status',
                    title='Status of the cylinder',
                    #type='text',
                    #value='',
                    disabled='True',
                    style={'fontSize':'large','textAlign':'center'}
                )
            ,className="five columns")
        ],className='row')
    ])    
],className="ten columns offset-by-one")


def average(alist):
    if (len(alist) > 0):
        return sum(alist) / len(alist)
    else:
        return 0

@app.callback(
    [Output(component_id='measuredFreq', component_property='value'),
     Output(component_id='status', component_property='value'),
     Output(component_id='temp', component_property='value')],
    [Input(component_id='freq', component_property='value')]
)
def testCylinder(freq):
    ser = serial.Serial('COM11', baudrate=9600, timeout=1)
    frequencies = []
    for i in range(2):
        sum = 0
        j = 0
        ser.write(b'a')
        t_end = time.time() + 1
        while time.time() < t_end:
            for line in ser:
                sum += float((line.decode('ascii')[:-2]))
                j=j+1
        if (j > 0):
            frequencies.append(int(sum/j))
    averageFreq = int(average(frequencies))

    temperature = 0
    ser.write(b't')
    t_end = time.time() + 0.5
    while time.time() < t_end:
        for line in ser:
            temperature = float((line.decode('ascii')[:-2]))   
    ser.close()

    statusMessage = ''
    if (1000 < freq < 4500):
        massLoss = 0.4242*(freq-averageFreq)
        if (massLoss<=14):
            statusMessage = 'Cylinder is full.'
        else:
            statusMessage = 'Empty, mass loss = {:.0f} grams.'.format(massLoss)
    else:
        statusMessage = 'Invalid inputs.'

    return averageFreq, statusMessage, int(temperature)
    

if __name__ == '__main__':
    app.run_server(debug=False)
