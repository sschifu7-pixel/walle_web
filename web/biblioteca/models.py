from django.db import models
from django.utils import timezone

class Rol(models.Model):
    nombre_rol = models.CharField(max_length=50, verbose_name="Nombre del Rol")
    descripcion = models.CharField(max_length=150, blank=True, null=True, verbose_name="Descripción")

    class Meta:
        verbose_name = "Rol"
        verbose_name_plural = "Roles"

    def __str__(self):
        return self.nombre_rol


class Alumno(models.Model):
    nfc_uid = models.CharField(max_length=50, primary_key=True, verbose_name="NFC UID (Addr04)")
    numero_control = models.CharField(max_length=20, unique=True, verbose_name="Matrícula / N° Control")
    nombre = models.CharField(max_length=100, verbose_name="Nombre Completo")
    rol = models.ForeignKey(Rol, on_delete=models.SET_NULL, null=True, blank=True, verbose_name="Rol")
    acceso_permitido = models.BooleanField(default=True, verbose_name="Acceso Permitido")

    class Meta:
        verbose_name = "Alumno"
        verbose_name_plural = "Alumnos"

    def __str__(self):
        return f"{self.nombre} ({self.numero_control})"


class Area(models.Model):
    nombre_area = models.CharField(max_length=100, verbose_name="Nombre del Área")
    ubicacion = models.CharField(max_length=100, verbose_name="Ubicación")

    class Meta:
        verbose_name = "Área"
        verbose_name_plural = "Áreas"

    def __str__(self):
        return f"{self.nombre_area} - {self.ubicacion}"


class Libro(models.Model):
    ESTADOS = [
        ('Disponible', 'Disponible'),
        ('Prestado', 'Prestado'),
        ('En Reparacion', 'En Reparación'),
    ]

    nfc_uid = models.CharField(max_length=50, unique=True, null=True, blank=True, verbose_name="NFC UID (Addr04)")
    titulo = models.CharField(max_length=150, verbose_name="Título del Libro")
    autor = models.CharField(max_length=100, verbose_name="Autor")
    isbn = models.CharField(max_length=20, unique=True, null=True, blank=True, verbose_name="ISBN")
    area = models.ForeignKey(Area, on_delete=models.SET_NULL, null=True, blank=True, verbose_name="Área")
    estado = models.CharField(max_length=20, choices=ESTADOS, default='Disponible', verbose_name="Estado")

    class Meta:
        verbose_name = "Libro"
        verbose_name_plural = "Libros"

    def __str__(self):
        return f"{self.titulo} - {self.autor} [{self.estado}]"


class RegistroAcceso(models.Model):
    alumno = models.ForeignKey(Alumno, on_delete=models.CASCADE, verbose_name="Alumno")
    area = models.ForeignKey(Area, on_delete=models.SET_NULL, null=True, blank=True, verbose_name="Área")
    fecha_hora = models.DateTimeField(default=timezone.now, verbose_name="Fecha y Hora")

    class Meta:
        verbose_name = "Registro de Acceso"
        verbose_name_plural = "Registros de Acceso"
        ordering = ['-fecha_hora']

    def __str__(self):
        return f"Acceso de {self.alumno.nombre} a las {self.fecha_hora.strftime('%H:%M %d/%m/%Y')}"


class Prestamo(models.Model):
    libro = models.ForeignKey(Libro, on_delete=models.CASCADE, verbose_name="Libro")
    alumno = models.ForeignKey(Alumno, on_delete=models.CASCADE, verbose_name="Alumno")
    fecha_prestamo = models.DateTimeField(default=timezone.now, verbose_name="Fecha de Préstamo")
    fecha_devolucion = models.DateTimeField(null=True, blank=True, verbose_name="Fecha de Devolución")

    class Meta:
        verbose_name = "Préstamo"
        verbose_name_plural = "Préstamos"
        ordering = ['-fecha_prestamo']

    def __str__(self):
        estado_str = "Devuelto" if self.fecha_devolucion else "Activo"
        return f"Préstamo: {self.libro.titulo} -> {self.alumno.nombre} ({estado_str})"
