from django.contrib import admin
from .models import Rol, Alumno, Area, Libro, RegistroAcceso, Prestamo

@admin.register(Rol)
class RolAdmin(admin.ModelAdmin):
    list_display = ('id', 'nombre_rol', 'descripcion')
    search_fields = ('nombre_rol',)

@admin.register(Alumno)
class AlumnoAdmin(admin.ModelAdmin):
    list_display = ('nfc_uid', 'numero_control', 'nombre', 'rol', 'acceso_permitido')
    list_filter = ('acceso_permitido', 'rol')
    search_fields = ('nfc_uid', 'numero_control', 'nombre')

@admin.register(Area)
class AreaAdmin(admin.ModelAdmin):
    list_display = ('id', 'nombre_area', 'ubicacion')
    search_fields = ('nombre_area', 'ubicacion')

@admin.register(Libro)
class LibroAdmin(admin.ModelAdmin):
    list_display = ('id', 'nfc_uid', 'titulo', 'autor', 'isbn', 'estado', 'area')
    list_filter = ('estado', 'area')
    search_fields = ('nfc_uid', 'titulo', 'autor', 'isbn')

@admin.register(RegistroAcceso)
class RegistroAccesoAdmin(admin.ModelAdmin):
    list_display = ('id', 'alumno', 'area', 'fecha_hora')
    list_filter = ('area', 'fecha_hora')
    search_fields = ('alumno__nombre', 'alumno__numero_control')

@admin.register(Prestamo)
class PrestamoAdmin(admin.ModelAdmin):
    list_display = ('id', 'libro', 'alumno', 'fecha_prestamo', 'fecha_devolucion')
    list_filter = ('fecha_prestamo', 'fecha_devolucion')
    search_fields = ('libro__titulo', 'alumno__nombre', 'alumno__numero_control')
