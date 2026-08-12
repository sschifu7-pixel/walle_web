#!/usr/bin/env bash
# exit on error
set -o errexit

pip install -r requirements.txt

python manage.py collectstatic --no-input
python manage.py migrate
python manage.py shell -c "from biblioteca.models import Rol, Alumno, Area, Libro; r, _ = Rol.objects.get_or_create(nombre_rol='Estudiante'); a, _ = Area.objects.get_or_create(nombre_area='Biblioteca General', ubicacion='Edificio A'); Alumno.objects.get_or_create(nfc_uid='A1B2C3D4', defaults={'numero_control':'20210045', 'nombre':'Juan Pérez González', 'rol':r}); Alumno.objects.get_or_create(nfc_uid='E5F6A7B8', defaults={'numero_control':'20210088', 'nombre':'María López Hernández', 'rol':r}); Libro.objects.get_or_create(nfc_uid='11223344', defaults={'titulo':'Cien Años de Soledad', 'autor':'Gabriel García Márquez', 'isbn':'9780307474728', 'area':a}); Libro.objects.get_or_create(nfc_uid='55667788', defaults={'titulo':'El Principito', 'autor':'Antoine de Saint-Exupéry', 'isbn':'9780156013987', 'area':a}); from django.contrib.auth.models import User; User.objects.filter(username='admin').exists() or User.objects.create_superuser('admin', 'admin@walle.com', 'admin123')"
