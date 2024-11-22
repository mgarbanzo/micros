import streamlit as st
import folium
from streamlit_folium import st_folium
identificador="A"
# Configuración de la página de Streamlit
st.set_page_config(layout="wide", page_title="Mapa de Costa Rica")

# Coordenadas de las posiciones en Costa Rica
positions = [
    (9.9350, -84.1919),  # San José
    (10.0167, -84.2167), # Alajuela
    (10.6356, -85.4403)  # Liberia
]

# Crear el mapa centrado en la primera posición
m = folium.Map(location=(9.834066260020085, -83.91726983862638), zoom_start=11)
latc=9.834066260020085
lonc=-83.91726983862638
n=0.2
square_coordinates = [
    [latc - n, lonc - n],
    [latc - n, lonc + n],
    [latc + n, lonc + n],
    [latc + n, lonc - n],
    [latc - n, lonc - n]  # Volver al punto inicial para cerrar el polígono
]
# Dibujar líneas entre las posiciones
folium.PolyLine(positions, color="blue", weight=2.5, opacity=1).add_to(m)
ls = folium.PolyLine(
    locations=square_coordinates, color="red"
)
folium.Marker(
    location=[latc, lonc],
    popup="Centro",
    icon=folium.Icon(icon='star', prefix='fa', color='blue')
).add_to(m)

ls.add_child(folium.Popup("limites hacienda"))
ls.add_to(m)
# Agregar marcadores para las posiciones
for i, pos in enumerate(positions):
    if i == len(positions) - 1:
        # Último punto con letra roja
        folium.Marker(
            location=pos,
            icon=folium.DivIcon(html=f"""<div style="font-size: 16px; color: red;">{identificador}</div>""")
        ).add_to(m)
    else:
        # Puntos azules
        folium.CircleMarker(
            location=pos,
            radius=5,
            color="blue",
            fill=True,
            fill_color="blue",
            fill_opacity=0.8
        ).add_to(m)

# Crear columnas para expandir el mapa a toda la pantalla
st.title("Mapa de Costa Rica con trayecto")
    # Mostrar el mapa ajustado
st_folium(m, width=2200, height=800)  # Ajusta el tamaño del mapa

