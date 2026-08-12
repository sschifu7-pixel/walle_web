import json
from django.shortcuts import render, redirect, get_object_or_404
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.utils import timezone
from django.db import transaction
from .models import Rol, Alumno, Area, Libro, RegistroAcceso, Prestamo

def dashboard_view(request):
    """
    Vista principal de la Interfaz Wall-E OS (Consola LCD y Transacciones NFC)
    """
    prestamos_recientes = Prestamo.objects.select_related('libro', 'alumno').all()[:10]
    accesos_recientes = RegistroAcceso.objects.select_related('alumno', 'area').all()[:10]
    
    total_libros = Libro.objects.count()
    libros_prestados = Libro.objects.filter(estado='Prestado').count()
    libros_disponibles = Libro.objects.filter(estado='Disponible').count()
    total_alumnos = Alumno.objects.count()

    context = {
        'prestamos_recientes': prestamos_recientes,
        'accesos_recientes': accesos_recientes,
        'total_libros': total_libros,
        'libros_prestados': libros_prestados,
        'libros_disponibles': libros_disponibles,
        'total_alumnos': total_alumnos,
    }
    return render(request, 'dashboard.html', context)


def libros_view(request):
    """
    Catálogo y registro rápido de libros
    """
    if request.method == 'POST':
        nfc_uid = request.POST.get('nfc_uid', '').strip().upper()
        titulo = request.POST.get('titulo', '').strip()
        autor = request.POST.get('autor', '').strip()
        isbn = request.POST.get('isbn', '').strip()
        
        if titulo and autor:
            Libro.objects.create(
                nfc_uid=nfc_uid if nfc_uid else None,
                titulo=titulo,
                autor=autor,
                isbn=isbn if isbn else None,
                estado='Disponible'
            )
            return redirect('libros')

    query = request.GET.get('q', '')
    if query:
        libros = Libro.objects.filter(titulo__icontains=query) | Libro.objects.filter(nfc_uid__icontains=query) | Libro.objects.filter(autor__icontains=query)
    else:
        libros = Libro.objects.all()

    return render(request, 'libros.html', {'libros': libros, 'query': query})


def alumnos_view(request):
    """
    Directorio y alta rápida de alumnos
    """
    if request.method == 'POST':
        nfc_uid = request.POST.get('nfc_uid', '').strip().upper()
        numero_control = request.POST.get('numero_control', '').strip()
        nombre = request.POST.get('nombre', '').strip()

        if nfc_uid and numero_control and nombre:
            Alumno.objects.create(
                nfc_uid=nfc_uid,
                numero_control=numero_control,
                nombre=nombre,
                acceso_permitido=True
            )
            return redirect('alumnos')

    query = request.GET.get('q', '')
    if query:
        alumnos = Alumno.objects.filter(nombre__icontains=query) | Alumno.objects.filter(numero_control__icontains=query) | Alumno.objects.filter(nfc_uid__icontains=query)
    else:
        alumnos = Alumno.objects.all()

    return render(request, 'alumnos.html', {'alumnos': alumnos, 'query': query})


def historial_view(request):
    """
    Historial completo de préstamos y accesos
    """
    prestamos = Prestamo.objects.select_related('libro', 'alumno').all()[:50]
    accesos = RegistroAcceso.objects.select_related('alumno', 'area').all()[:50]
    return render(request, 'historial.html', {'prestamos': prestamos, 'accesos': accesos})


@csrf_exempt
def api_procesar_nfc(request):
    """
    Endpoint API REST / AJAX para procesar lecturas NFC (Addr04) en tiempo real
    Acepta tanto JSON como Form-Data.
    """
    if request.method != 'POST':
        return JsonResponse({'status': 'error', 'message': 'Método no permitido. Use POST.'}, status=405)

    try:
        if request.content_type == 'application/json':
            data = json.loads(request.body)
        else:
            data = request.POST

        nfc_uid = str(data.get('nfc_uid', '')).strip().upper()
        modo = data.get('modo', 'prestamo')  # "prestamo" o "devolucion"
        alumno_uid_memoria = data.get('alumno_uid_memoria', None)

        if not nfc_uid:
            return JsonResponse({'status': 'error', 'message': 'Se requiere parámetro nfc_uid.'}, status=400)

        area_defecto = Area.objects.first()

        # MODO PRÉSTAMO
        if modo == 'prestamo':
            # 1. Buscar si el NFC pertenece a un alumno
            alumno = Alumno.objects.filter(nfc_uid=nfc_uid).first()
            if alumno:
                if alumno.acceso_permitido:
                    # Registrar acceso a la biblioteca
                    RegistroAcceso.objects.create(alumno=alumno, area=area_defecto)
                    return JsonResponse({
                        'status': 'success',
                        'type': 'alumno',
                        'alumno_uid': alumno.nfc_uid,
                        'nombre': alumno.nombre,
                        'matricula': alumno.numero_control,
                        'message': f"ALUMNO: {alumno.nombre}\nMAT: {alumno.numero_control}\n▶ ESCANEE LIBRO AHORA"
                    })
                else:
                    return JsonResponse({
                        'status': 'error',
                        'type': 'alumno',
                        'message': f"ACCESO DENEGADO\nALUMNO INACTIVO ({alumno.nombre})"
                    })

            # 2. Buscar si el NFC pertenece a un libro
            libro = Libro.objects.filter(nfc_uid=nfc_uid).first()
            if libro:
                if alumno_uid_memoria:
                    alumno = Alumno.objects.filter(nfc_uid=alumno_uid_memoria).first()
                    if not alumno:
                        return JsonResponse({
                            'status': 'error',
                            'type': 'libro',
                            'message': "ERROR: ALUMNO EN MEMORIA NO VALIDO"
                        })

                    if libro.estado == 'Disponible':
                        with transaction.atomic():
                            Prestamo.objects.create(libro=libro, alumno=alumno)
                            libro.estado = 'Prestado'
                            libro.save()

                        return JsonResponse({
                            'status': 'success',
                            'type': 'prestamo_completado',
                            'libro': libro.titulo,
                            'alumno': alumno.nombre,
                            'message': f"PRÉSTAMO ÉXITOSO!\nLIBRO: [{libro.titulo}]"
                        })
                    else:
                        return JsonResponse({
                            'status': 'error',
                            'type': 'libro',
                            'message': f"LIBRO NO DISPONIBLE\nESTADO: {libro.estado}"
                        })
                else:
                    return JsonResponse({
                        'status': 'error',
                        'type': 'libro',
                        'message': "ESCANEE CREDENCIAL\nDE ALUMNO PRIMERO"
                    })

            # 3. No registrado
            return JsonResponse({
                'status': 'error',
                'type': 'desconocido',
                'message': f"CÓDIGO ADDR04 NO REGISTRADO\nUID: {nfc_uid}"
            })

        # MODO DEVOLUCIÓN
        else:
            libro = Libro.objects.filter(nfc_uid=nfc_uid).first()
            if libro:
                if libro.estado == 'Prestado':
                    with transaction.atomic():
                        prestamo_activo = Prestamo.objects.filter(libro=libro, fecha_devolucion__isnull=True).first()
                        if prestamo_activo:
                            prestamo_activo.fecha_devolucion = timezone.now()
                            prestamo_activo.save()

                        libro.estado = 'Disponible'
                        libro.save()

                    return JsonResponse({
                        'status': 'success',
                        'type': 'devolucion_completada',
                        'libro': libro.titulo,
                        'message': f"DEVOLUCIÓN ÉXITOSA!\nLIBRO: [{libro.titulo}]"
                    })
                elif libro.estado == 'Disponible':
                    return JsonResponse({
                        'status': 'error',
                        'type': 'libro',
                        'message': f"EL LIBRO YA ESTÁ DISPONIBLE\n[{libro.titulo}]"
                    })
                else:
                    return JsonResponse({
                        'status': 'error',
                        'type': 'libro',
                        'message': f"ESTADO DEL LIBRO: {libro.estado}"
                    })
            else:
                return JsonResponse({
                    'status': 'error',
                    'type': 'desconocido',
                    'message': f"LIBRO NO REGISTRADO\nADDR04: {nfc_uid}"
                })

    except Exception as e:
        return JsonResponse({'status': 'error', 'message': f"Error interno en servidor: {str(e)}"}, status=500)
