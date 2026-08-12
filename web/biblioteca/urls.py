from django.urls import path
from . import views

urlpatterns = [
    path('', views.dashboard_view, name='dashboard'),
    path('libros/', views.libros_view, name='libros'),
    path('alumnos/', views.alumnos_view, name='alumnos'),
    path('historial/', views.historial_view, name='historial'),
    path('api/nfc/scan/', views.api_procesar_nfc, name='api_nfc_scan'),
    path('api/esp32/push/', views.api_procesar_nfc, name='api_esp32_push'),
]

