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
m = folium.Map(location=positions[0], zoom_start=8)

# Dibujar líneas entre las posiciones
folium.PolyLine(positions, color="blue", weight=2.5, opacity=1).add_to(m)

# Agregar marcadores para las posiciones
for i, pos in enumerate(positions):
    if i == len(positions) - 1:
        # Último punto con letra roja
        folium.Marker(
            location=pos,
            icon=folium.DivIcon(html=f"""<div style="font-size: 16px; color: red;">{identi>
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
