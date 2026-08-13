from django.db import models
from django.utils import timezone

class Rol(models.Model):
    id_rol = models.AutoField(primary_key=True, db_column='id_rol')
    nombre_rol = models.CharField(max_length=50, db_column='nombre_rol', verbose_name="Nombre del Rol")
    descripcion = models.CharField(max_length=150, blank=True, null=True, db_column='descripcion', verbose_name="Descripción")

    class Meta:
        db_table = 'roles'
        verbose_name = "Rol"
        verbose_name_plural = "Roles"

    def __str__(self):
        return self.nombre_rol


class Alumno(models.Model):
    nfc_uid = models.CharField(max_length=50, primary_key=True, db_column='nfc_uid', verbose_name="NFC UID (Addr04)")
    numero_control = models.CharField(max_length=15, db_column='numero_control', verbose_name="Matrícula / N° Control")
    nombre = models.CharField(max_length=100, db_column='nombre', verbose_name="Nombre Completo")
    rol = models.ForeignKey(Rol, on_delete=models.SET_NULL, null=True, blank=True, db_column='id_rol', verbose_name="Rol")
    acceso_permitido = models.BooleanField(default=True, db_column='acceso_permitido', verbose_name="Acceso Permitido")

    class Meta:
        db_table = 'alumnos'
        verbose_name = "Alumno"
        verbose_name_plural = "Alumnos"

    def __str__(self):
        return f"{self.nombre} ({self.numero_control})"


class Area(models.Model):
    id_area = models.AutoField(primary_key=True, db_column='id_area')
    nombre_area = models.CharField(max_length=100, db_column='nombre_area', verbose_name="Nombre del Área")
    ubicacion = models.CharField(max_length=100, db_column='ubicacion', verbose_name="Ubicación")

    class Meta:
        db_table = 'areas'
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

    id_libro = models.AutoField(primary_key=True, db_column='id_libro')
    nfc_uid = models.CharField(max_length=50, null=True, blank=True, db_column='nfc_uid', verbose_name="NFC UID (Addr04)")
    titulo = models.CharField(max_length=150, db_column='titulo', verbose_name="Título del Libro")
    autor = models.CharField(max_length=100, db_column='autor', verbose_name="Autor")
    isbn = models.CharField(max_length=20, null=True, blank=True, db_column='isbn', verbose_name="ISBN")
    area = models.ForeignKey(Area, on_delete=models.SET_NULL, null=True, blank=True, db_column='id_area', verbose_name="Área")
    estado = models.CharField(max_length=20, choices=ESTADOS, default='Disponible', db_column='estado', verbose_name="Estado")

    class Meta:
        db_table = 'libros'
        verbose_name = "Libro"
        verbose_name_plural = "Libros"

    def __str__(self):
        return f"{self.titulo} - {self.autor} [{self.estado}]"


class RegistroAcceso(models.Model):
    id_acceso = models.AutoField(primary_key=True, db_column='id_acceso')
    alumno = models.ForeignKey(Alumno, on_delete=models.CASCADE, db_column='nfc_uid', verbose_name="Alumno")
    area = models.ForeignKey(Area, on_delete=models.SET_NULL, null=True, blank=True, db_column='id_area', verbose_name="Área")
    fecha_hora = models.DateTimeField(default=timezone.now, db_column='fecha_hora', verbose_name="Fecha y Hora")

    class Meta:
        db_table = 'registros_acceso'
        verbose_name = "Registro de Acceso"
        verbose_name_plural = "Registros de Acceso"
        ordering = ['-fecha_hora']

    def __str__(self):
        return f"Acceso de {self.alumno.nombre} a las {self.fecha_hora.strftime('%H:%M %d/%m/%Y')}"


class Prestamo(models.Model):
    id_prestamo = models.AutoField(primary_key=True, db_column='id_prestamo')
    libro = models.ForeignKey(Libro, on_delete=models.CASCADE, db_column='id_libro', verbose_name="Libro")
    alumno = models.ForeignKey(Alumno, on_delete=models.CASCADE, db_column='nfc_uid', verbose_name="Alumno")
    fecha_prestamo = models.DateTimeField(default=timezone.now, db_column='fecha_prestamo', verbose_name="Fecha de Préstamo")
    fecha_devolucion = models.DateTimeField(null=True, blank=True, db_column='fecha_devolucion', verbose_name="Fecha de Devolución")

    class Meta:
        db_table = 'prestamos'
        verbose_name = "Préstamo"
        verbose_name_plural = "Préstamos"
        ordering = ['-fecha_prestamo']

    def __str__(self):
        estado_str = "Devuelto" if self.fecha_devolucion else "Activo"
        return f"Préstamo: {self.libro.titulo} -> {self.alumno.nombre} ({estado_str})"
