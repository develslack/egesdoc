console.clear();
console.log("✅ Módulo Normas Iniciado de forma nativa.");

// CORRECCIÓN: Usamos el objeto window para evitar el SyntaxError de redeteclaración con let
window.dTable = window.dTable || null;

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL +"/normas/list");
    const registros = await response.json();

    const tableBody = document.getElementById("normasTableBody");
    const alertInfo = document.getElementById("alert-info-normas");
    tableBody.innerHTML = "";

    let count = 0;

    registros.forEach((reg) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td class="text-center">${reg.nombre_norma}</td>
        <td class="text-center">${reg.n_norma}</td>
        <td class="text-center">${reg.tipo_norma}</td>
        <td class="text-center">${reg.f_norma}</td>
        <td class="text-center">${reg.f_pub}</td>
        <td class="text-center">${reg.anio_pub}</td>
        <td class="text-center">${reg.jurisdiccion}</td>
        <td class="text-center">${reg.organismo}</td>
        <td class="text-center">${reg.descripcion_organismo}</td>
        <td class="text-center">${reg.ubicacion_fisica}</td>
        <td class="text-center">
          <button class="btn btn-default btn-sm btn-editar-norma"
                  data-id="${reg.id}"
                  data-nombre_norma="${reg.nombre_norma}"
                  data-n_norma="${reg.n_norma}"
                  data-tipo_norma="${reg.tipo_norma}"
                  data-f_norma="${reg.f_norma}"
                  data-f_pub="${reg.f_pub}"
                  data-anio_pub="${reg.anio_pub}"
                  data-jurisdiccion="${reg.jurisdiccion}"
                  data-organismo="${reg.organismo}"
                  data-ubicacion_fisica="${reg.ubicacion_fisica}"
                  data-observaciones="${reg.observaciones}">
            <span class="glyphicon glyphicon-edit"></span> Editar
          </button>
        </td>`;

      tableBody.appendChild(row);
      count++;
    });

    if (alertInfo) {
        alertInfo.innerHTML = `<div class="alert alert-info">
                                <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                               </div><hr>`;
    }

    if ($.fn.DataTable.isDataTable("#normasTable")) {
      $("#normasTable").DataTable().destroy();
    }

    dTable = $('#normasTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender":    true,
        "retrieve":       true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            { extend: 'excel', text: '<i class="glyphicon glyphicon-list-alt"></i> Export Excel', exportOptions: { columns: ':visible'} },
            { extend: 'csv', text: '<i class="glyphicon glyphicon-align-justify"></i> Export CSV', exportOptions: { columns: ':visible'} },
            { extend: 'pdf', text: '<i class="glyphicon glyphicon-file"></i> Export PDF', exportOptions: { columns: ':visible'} },
            {
                extend: 'print',
                text: '<i class="glyphicon glyphicon-print"></i> Imprimir',
                customize: function ( win ) {
                    $(win.document.body).css( 'font-size', '8pt' );$(win.document.body).find( 'table' ).addClass( 'compact' ).css( 'font-size', 'inherit' );
                },
                autoPrint: false,
                exportOptions: { columns: ':visible' }
            },
            'colvis'
        ],
        "language": {
            "lengthMenu": "Mostrar _MENU_ registros por pagina",
            "info": "Mostrando pagina _PAGE_ de _PAGES_",
            "search": "Buscar:",
            "zeroRecords":    "No se encontraron registros coincidentes",
            "paginate": { "next": "Siguiente", "previous": "Anterior" }
        }
    });

    // ================================================================================================================= //
    // 2. PREPARACIÓN DEL MODAL USANDO EL SISTEMA DEL DASHBOARD
    // ================================================================================================================= //
    // Usamos el fetch nativo del servidor para rellenar el div de abajo sin pisar el dashboard central
    const contenedorModal = document.getElementById("contenedor-modal-maestro-normas");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/normas/nueva_norma.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    // ================================================================================================================= //
    // 3. CAPTURA SEGURO DE EVENTOS (DELEGACIÓN DE EVENTOS JQUERY)
    // ================================================================================================================= //

    // Evento Añadir (Alta)
    $(document).off("click", "#add-norma-form").on("click", "#add-norma-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    // Evento Editar (Fila de la tabla)
    $(document).off("click", ".btn-editar-norma").on("click", ".btn-editar-norma", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre_norma: $(this).attr("data-nombre_norma"),
            n_norma: $(this).attr("data-n_norma"),
            tipo_norma: $(this).attr("data-tipo_norma"),
            f_norma: $(this).attr("data-f_norma"),
            f_pub: $(this).attr("data-f_pub"),
            anio_pub: $(this).attr("data-anio_pub"),
            jurisdiccion: $(this).attr("data-jurisdiccion"),
            organismo: $(this).attr("data-organismo"),
            ubicacion_fisica: $(this).attr("data-ubicacion_fisica"),
            observaciones: $(this).attr("data-observaciones")
        };
        prepararYMostrarModal("edicion", datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Funciones Ejecutivas:", error);
  }
})();

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUYE EL FORMULARIO INTERNO Y MUESTRA EL MODAL FLOTANTE
// ===================================================================================================================== //
function prepararYMostrarModal(modo, datos = null) {
    const $modal =$("#myModal-normas");
    if ($modal.length === 0) return;

    // Seteamos título del modal flotante
    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Norma" : "Editar Norma");

    // Seteamos campos en el body vacío
    $modal.find(".modal-body").html(`

        <form id="form-modal-norma">
            <input type="hidden" id="modal-norma-id" value="${datos ? datos.id : ''}">
            <div class="form-group">
                <label>Nombre Norma:</label>
                <input type="text" class="form-control" id="modal-norma-nombre_norma" value="${datos ? datos.nombre_norma : ''}" required>
            </div>
            <div class="form-group">
                <label>Nro. Norma:</label>
                <input type="text" class="form-control" id="modal-norma-norma" value="${datos ? datos.n_norma : ''}" required>
            </div>
            <div class="form-group">
                <label>Valor UR:</label>
                <input type="text" class="form-control" id="modal-norma-valor_ur" value="${datos ? datos.valor_ur : ''}" required>
            </div>
            <div class="form-group">
                <label>Fecha Publicación:</label>
                <input type="date" class="form-control" id="modal-norma-f_pub" value="${datos ? datos.f_pub : ''}" required>
            </div>
            <div class="form-group">
                <label>Año Publicación:</label>
                <input type="text" class="form-control" id="modal-norma-anio_pub" value="${datos ? datos.anio_pub : ''}" maxlength="4" required>
            </div>
            <div class="form-group">
                <label>Ubicacion Física:</label>
                <input type="text" class="form-control" id="modal-norma-ubicacion_fisica" value="${datos ? datos.ubicacion_fisica : ''}" required>
            </div>
            <div class="form-group">
                <label>Observaciones:</label>
                <textarea class="form-control" id="modal-norma-observaciones" required>${datos ? datos.observaciones : ''}</textarea>
            </div>
            <div class="form-group">
                <label>Seleccione Archivo de Norma (PDF):</label>
                <input type="file" class="form-control" id="modal-norma-file_name" value="${datos ? datos.file_name : ''}" required>
            </div>

            <button type="submit" id="btn-submit-oculto" style="display:none;"></button>
        </form>
    `);

    // Seteamos barra de estado y botones en el footer vacío
    $modal.find(".modal-footer").html(`
        <div id="modal-status-message" style="margin-bottom: 10px; text-align: left;"></div>
        <div class="text-right">
            <button type="button" class="btn btn-danger" data-dismiss="modal">
                <span class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span> Cerrar</button>
            <button type="button" class="btn btn-success" id="btn-modal-guardar">
                <span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Guardar</button>
        </div>
    `);

    // Hacemos que aparezca de forma flotante sobre dashboard.html
    $modal.modal("show");



    // Vinculamos el botón verde del footer con la validación del form
    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    // Procesamiento del submit al backend en C
    document.getElementById("form-modal-norma").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        const id = document.getElementById("modal-norma-id").value;

        const payload = {
            nombre_norma: document.getElementById("modal-norma-nombre_norma").value,
            n_norma: document.getElementById("modal-norma-n_norma").value,
            tipo_norma: document.getElementById("modal-norma-tipo_norma").value,
            f_pub: document.getElementById("modal-norma-f_pub").value,
            anio_pub: document.getElementById("modal-norma-anio_pub").value,
            jurisdiccion: document.getElementById("modal-norma-jurisdiccion").value,
            organismo: document.getElementById("modal-norma-organismo").value,
            ubicacion_fisica: document.getElementById("modal-norma-ubicacion_fisica").value,
            observaciones: document.getElementById("modal-norma-observaciones").value,
            file_name: document.getElementById("modal-norma-file_name").value
        };

        if(id == ""){

            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/normas/add", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `nombre_norma=${encodeURIComponent(payload.nombre_norma)}&n_norma=${encodeURIComponent(payload.n_norma)}&tipo_norma=${encodeURIComponent(payload.tipo_norma)}&f_norma=${encodeURIComponent(payload.f_norma)}&f_pub=${encodeURIComponent(payload.f_pub)}&anio_pub=${encodeURIComponent(payload.anio_pub)}&ubicacion_fisica=${encodeURIComponent(payload.ubicacion_fisica)}&observaciones=${encodeURIComponent(payload.observaciones)}&file_name=${encodeURIComponent(payload.file_name)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/normas/normas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Ha ocurrido un problema al procesar la solicitud.</span>`;
            }

        }
        if(id != ""){


            statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud...</span>`;

            try {
                const enviorresp = await fetch(window.API_BASE_URL +"/normas/edit", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `id=${encodeURIComponent(id)}&nombre_norma=${encodeURIComponent(payload.nombre_norma)}&n_norma=${encodeURIComponent(payload.n_norma)}&tipo_norma=${encodeURIComponent(payload.tipo_norma)}&f_norma=${encodeURIComponent(payload.f_norma)}&f_pub=${encodeURIComponent(payload.f_pub)}&anio_pub=${encodeURIComponent(payload.anio_pub)}&ubicacion_fisica=${encodeURIComponent(payload.ubicacion_fisica)}&observaciones=${encodeURIComponent(payload.observaciones)}&file_name=${encodeURIComponent(payload.file_name)}`
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;

                    // Forzamos la actualización de la vista actual llamando al dashboard de forma nativa
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/normas/normas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            }
            catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Ha ocurrido un problema al procesar la solicitud.</span>`;
            }

        }

    };
}
