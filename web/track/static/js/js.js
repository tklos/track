
$(document).ready(function() {

	/* Pagination */
	$("body").on("click", "a.a-paginator", function(event) {
		event.preventDefault();

		var obj = $(this);
		var div = obj.closest(".div-ajax");
		var table = div.find("table.table-ajax");

		$.ajax({
			type: "GET",
			url: obj.data("url"),
			contentType: "application/json; charset=utf-8",
			dataType: "json",
			beforeSend: function() {
				table.addClass("table-inactive");
			},
			success: function(data) {
				div.html(data.html);
			},
			error: function(data) {
				alert("Request failed (error " + data.status + ": " + data.statusText + "); please reload page");
			}
		});
	});


	/* Device delete buttons */
	$("button.device-delete").click(function(event) {
		var obj = $(this);
		var form = obj.siblings("form").first();

		form.submit();
	});


});

