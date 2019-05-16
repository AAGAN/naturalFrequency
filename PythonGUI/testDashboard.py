import dash
from dash.dependencies import Output, Input
import dash_core_components as dcc
import dash_html_components as html
from collections import deque
import serial
import time

ser = serial.Serial('COM13', baudrate=9600, timeout=1)

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
    html.Div(
        dcc.Input(
            id = 'freq',
            placeholder='Initial Frequency (Hz)',
            type='number',
            #value='2000',
            debounce='True'
        )
    ),
    html.Div(
        dcc.Input(
            id='temp',
            placeholder='Temperature (C)',
            type='number',
            #value='23',
            debounce='True'
        )
    ),
    html.Div(
        dcc.Input(
            id='status',
            placeholder='Status',
            type='text',
            value='',
            disabled='True'
        )
    ),
    html.Div(
        dcc.Textarea(
            id='textarea',
            placeholder='measured frequencies',
            value='text area',
            style={'width':'100%'}
        )
    )

    
])

# def measure():
#     for i in range(5):
#         print(i)
#         ser.write(b'a')
#         t_end = time.time() + 1
#         while time.time() < t_end:
#             for line in ser:
#                 print(line.decode('ascii')[:-2])
#     ser.close()

@app.callback(
    Output(component_id='textarea', component_property='value'),
    [Input(component_id='freq',component_property='value'),Input(component_id='temp',component_property='value')]
)
def testCylinder(freq,temp):
    frequencies = []
    for i in range(5):
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
    return frequencies

if __name__ == '__main__':
    app.run_server(debug=False)
